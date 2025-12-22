#include "ffmpegsubtitleanalyzer.h"

struct SubtitleItem {
    QString text;
    qint64 startMs;
    qint64 endMs;
};

FFmpegSubtitleAnalyzer::FFmpegSubtitleAnalyzer(const QString &path, QObject *parent)
    : QThread(parent), filePath(path) {
    targetPositionMs.store(0);
    isRunning.store(false);
    selectedStreamIndex.store(-1);
}

FFmpegSubtitleAnalyzer::~FFmpegSubtitleAnalyzer() {
    requestInterruption();
    wait();
}

void FFmpegSubtitleAnalyzer::seekTo(qint64 positionMs) {
    QMutexLocker locker(&seekMutex);
    targetPositionMs.store(positionMs);
}

void FFmpegSubtitleAnalyzer::toggleRunning(bool running) {
    isRunning.store(running);
}

void FFmpegSubtitleAnalyzer::selectTrack(int streamIndex) {
    selectedStreamIndex.store(streamIndex);
    seekTo(targetPositionMs.load()); 
    if (streamIndex == -1) emit subtitleDataAvailable("");
}

void FFmpegSubtitleAnalyzer::findSubtitleStreams(AVFormatContext *fmt) {
    availableTracks.clear();
    for (unsigned int i = 0; i < fmt->nb_streams; ++i) {
        if (fmt->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
            SubtitleTrack track;
            track.streamIndex = i;
            AVDictionaryEntry *lang = av_dict_get(fmt->streams[i]->metadata, "language", NULL, 0);
            track.language = lang ? QString::fromUtf8(lang->value) : "Unknown";
            availableTracks.append(track);
        }
    }
}

void FFmpegSubtitleAnalyzer::run() {
    AVFormatContext *fmt = nullptr;
    if (avformat_open_input(&fmt, filePath.toUtf8().data(), nullptr, nullptr) < 0) return;
    avformat_find_stream_info(fmt, nullptr);

    findSubtitleStreams(fmt);

    AVCodecContext *codecCtx = nullptr;
    int currentActiveStream = -1;
    AVPacket *pkt = av_packet_alloc();
    
    QList<SubtitleItem> subtitleBuffer; 
    QString lastEmittedText = "";

    while (!isInterruptionRequested()) {
        int targetStream = selectedStreamIndex.load();
        qint64 masterClock = targetPositionMs.load() + 65; // + Thread latency compensation

        // ---- Handle Track Switching ----
        if (targetStream != currentActiveStream) {
            if (codecCtx) avcodec_free_context(&codecCtx);
            if (targetStream != -1) {
                const AVCodec *codec = avcodec_find_decoder(fmt->streams[targetStream]->codecpar->codec_id);
                if (codec) {
                    codecCtx = avcodec_alloc_context3(codec);
                    avcodec_parameters_to_context(codecCtx, fmt->streams[targetStream]->codecpar);
                    avcodec_open2(codecCtx, codec, nullptr);
                }
            }
            currentActiveStream = targetStream;
            subtitleBuffer.clear();
            lastEmittedText = "";
        }

        if (!isRunning.load() || currentActiveStream == -1 || !codecCtx) {
            QThread::msleep(50);
            continue;
        }

        // ---- Dynamic Buffering (Read Ahead) ----
        // Reads frames until having a few seconds of subtitles ahead of masterClock
        while (subtitleBuffer.size() < 10 && av_read_frame(fmt, pkt) >= 0) {
            if (pkt->stream_index == currentActiveStream) {
                AVSubtitle sub;
                int got_sub = 0;
                if (avcodec_decode_subtitle2(codecCtx, &sub, &got_sub, pkt) >= 0 && got_sub) {
                    AVStream *st = fmt->streams[currentActiveStream];
                    // PTS to MS conversion
                    qint64 ptsMs = 0;
                    if (pkt->pts != AV_NOPTS_VALUE)
                        ptsMs = av_rescale_q(pkt->pts, st->time_base, {1, 1000});

                    SubtitleItem item;
                    item.startMs = ptsMs + sub.start_display_time;
                    item.endMs = ptsMs + sub.end_display_time;
                    
                    // If end_display_time is not set by codec, default is 2s
                    if (sub.end_display_time == 0 || item.endMs <= item.startMs)
                        item.endMs = item.startMs + 2000;

                    // ---- Text Extraction ----
                    QString fullText = "";
                    for (unsigned int i = 0; i < sub.num_rects; i++) {
                        if (sub.rects[i]->ass) {
                            // ASS/SSA Format Cleanup
                            QString raw = QString::fromUtf8(sub.rects[i]->ass);
                            QString clean = raw.section(',', 8);
                            if (clean.startsWith(",")) clean = raw.section(',', 9);
                            clean.remove(QRegularExpression("\\{[^\\}]*\\}"));
                            fullText += clean.replace("\\N", "\n").replace("\\n", "\n");
                        } else if (sub.rects[i]->text) {
                            // SRT/Plain Text/UTF-8 Format Cleanup
                            QString raw = QString::fromUtf8(sub.rects[i]->text);
                            raw.remove(QRegularExpression("<[^>]*>")); 
                            fullText += raw;
                        }
                    }
                    
                    item.text = fullText.trimmed();
                    if (!item.text.isEmpty()) {
                        subtitleBuffer.append(item);
                        // Buffer sorted by start time: Ensures binary search or linear check works
                        std::sort(subtitleBuffer.begin(), subtitleBuffer.end(), 
                            [](const SubtitleItem& a, const SubtitleItem& b) { return a.startMs < b.startMs; });
                    }
                    avsubtitle_free(&sub);
                }
            }
            av_packet_unref(pkt);

            // Search up to 1000 packets ahead
            static int packetCounter = 0;
            if (++packetCounter > 1000) {
                packetCounter = 0;
                break;
            }
        }

        // ---- Seek Synchronization (Explicit Local Scope | Nested Block) ----
        {
            QMutexLocker locker(&seekMutex);
            // If buffer is empty or currently far away from buffer range, seek the file
            bool bufferRelevant = !subtitleBuffer.isEmpty() && 
                                  masterClock >= (subtitleBuffer.first().startMs - 5000) && 
                                  masterClock <= (subtitleBuffer.last().endMs + 1000);
            
            if (!bufferRelevant) {
                AVStream *st = fmt->streams[currentActiveStream];
                av_seek_frame(fmt, currentActiveStream, 
                              av_rescale_q(masterClock, {1, 1000}, st->time_base), 
                              AVSEEK_FLAG_BACKWARD);
                avcodec_flush_buffers(codecCtx);
                subtitleBuffer.clear();
            }
        }

        // ---- Visibility ----
        QString currentText = "";
        for (const auto& item : subtitleBuffer) {
            if (masterClock >= item.startMs && masterClock <= item.endMs)
                currentText = item.text;
        }

        if (currentText != lastEmittedText) {
            emit subtitleDataAvailable(currentText);
            lastEmittedText = currentText;
        }

        // ---- Cleanup ----
        subtitleBuffer.erase(std::remove_if(subtitleBuffer.begin(), subtitleBuffer.end(),
            [masterClock](const SubtitleItem& item) { return item.endMs < (masterClock - 2000); }), 
            subtitleBuffer.end());
        QThread::msleep(15); 
    }

    if (codecCtx) avcodec_free_context(&codecCtx);
    av_packet_free(&pkt);
    avformat_close_input(&fmt);
}