#include "ffmpegaudioanalyzer.h"

FFmpegAudioAnalyzer::FFmpegAudioAnalyzer(const QString &path, QObject *parent)
    : QThread(parent), filePath(path) {
    // FFTW buffers allocation
    m_fftIn = (double*) fftw_malloc(sizeof(double) * FFT_SIZE);
    m_fftOut = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (FFT_SIZE / 2 + 1));
    
    // FFTW plan creation
    m_fftPlan = fftw_plan_dft_r2c_1d(FFT_SIZE, m_fftIn, m_fftOut, FFTW_ESTIMATE);
}

FFmpegAudioAnalyzer::~FFmpegAudioAnalyzer() {
    requestInterruption();
    wait();

    // Cleanup FFTW resources
    if (m_fftPlan) fftw_destroy_plan(m_fftPlan);
    if (m_fftIn) fftw_free(m_fftIn);
    if (m_fftOut) fftw_free(m_fftOut);
}

void FFmpegAudioAnalyzer::seekTo(qint64 positionMs) {
    QMutexLocker locker(&seekMutex);
    targetPositionMs.store(positionMs);
}

void FFmpegAudioAnalyzer::toggleRunning(bool running) {
    isRunning.store(running);
    
    if (running) {
        QMutexLocker locker(&seekMutex);
        targetPositionMs.store(targetPositionMs.load());
    }
}

void FFmpegAudioAnalyzer::run() {
    AVFormatContext *fmt = nullptr;

    if (avformat_open_input(&fmt, filePath.toUtf8().data(), nullptr, nullptr) < 0)
        return;

    if (avformat_find_stream_info(fmt, nullptr) < 0)
        return;

    int audioStreamIndex = av_find_best_stream(fmt, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audioStreamIndex < 0)
        return;

    AVStream *stream = fmt->streams[audioStreamIndex];
    AVCodecParameters *codecpar = stream->codecpar;

    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec)
        return;

    AVCodecContext *codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx)
        return;

    if (avcodec_parameters_to_context(codecCtx, codecpar) < 0)
        return;

    if (avcodec_open2(codecCtx, codec, nullptr) < 0)
        return;

    // ---- Resampler Config ----
    const AVSampleFormat outFmt = AV_SAMPLE_FMT_S16;
    sampleRate = codecCtx->sample_rate;
    const int outChannels = 2;

    AVChannelLayout outChLayout;
    av_channel_layout_default(&outChLayout, outChannels);

    AVChannelLayout inChLayout;
    av_channel_layout_copy(&inChLayout, &codecpar->ch_layout);

    SwrContext *swr = nullptr;
    if (swr_alloc_set_opts2(
            &swr,
            &outChLayout,
            outFmt,
            sampleRate,
            &inChLayout,
            codecCtx->sample_fmt,
            sampleRate,
            0, nullptr
        ) < 0) {
        if (swr) swr_free(&swr);
        return;
    }

    if (swr_init(swr) < 0) {
        if (swr) swr_free(&swr);
        return;
    }

    // ---- Packets & Frames ----
    AVPacket *pkt = av_packet_alloc();
    AVFrame  *frame = av_frame_alloc();
    if (!pkt || !frame) {
        if (pkt) av_packet_free(&pkt);
        if (frame) av_frame_free(&frame);
        return;
    }

    QVector<int16_t> pcm;
    pcm.reserve(FFT_SIZE * outChannels * 3);

    // ---- Main Loop ----
    qint64 currentSampleOffset = 0; // Total samples processed

    while (!isInterruptionRequested()) {
        // ---- Pause/Resume Check ----
        if (!isRunning.load()) {
            QThread::msleep(100); // Brief sleep if paused
            continue;
        }

        // ---- Seek Check (Jump) ----
        qint64 currentPositionMs = (currentSampleOffset * 1000LL) / sampleRate;
        qint64 targetPos = targetPositionMs.load();

        if (qAbs(targetPos - currentPositionMs) > 500) {
            QMutexLocker locker(&seekMutex);

            // New time sample offset
            currentSampleOffset = (targetPos * (qint64)sampleRate) / 1000LL;
            
            // Converts time to stream timestamp and perform seek
            qint64 seek_ts = av_rescale(targetPos, stream->time_base.den, (qint64)stream->time_base.num * 1000LL);
            
            // Handles seeking
            if (av_seek_frame(fmt, audioStreamIndex, seek_ts, AVSEEK_FLAG_BACKWARD) >= 0) {
                // Clear remaining buffer and reset state after seek
                pcm.clear();
                avcodec_flush_buffers(codecCtx);
            }

            // Target position → Current position: Avoids immediate re-seek
            targetPositionMs.store((currentSampleOffset * 1000LL) / sampleRate);

            // Restarts the loop from the top: Checks if still paused
            continue;
        }

        // ---- Read, Decode & Resample ----
        if (av_read_frame(fmt, pkt) >= 0) {
            if (pkt->stream_index != audioStreamIndex) {
                av_packet_unref(pkt);
                continue;
            }

            if (avcodec_send_packet(codecCtx, pkt) < 0) {
                av_packet_unref(pkt);
                continue;
            }

            while (avcodec_receive_frame(codecCtx, frame) == 0) {
                int maxOutSamples = av_rescale_rnd(
                    frame->nb_samples,
                    sampleRate,
                    codecCtx->sample_rate,
                    AV_ROUND_UP
                ) + 32;

                uint8_t **outData = nullptr;
                int outLineSize = 0;

                if (av_samples_alloc_array_and_samples(
                        &outData,
                        &outLineSize,
                        outChannels,
                        maxOutSamples,
                        outFmt,
                        0) < 0) {
                    av_frame_unref(frame);
                    continue;
                }

                int converted = swr_convert(
                    swr,
                    outData,
                    maxOutSamples,
                    (const uint8_t**)frame->extended_data,
                    frame->nb_samples
                );

                if (converted < 0) {
                    av_freep(&outData[0]);
                    av_freep(&outData);
                    av_frame_unref(frame);
                    continue;
                }

                int totalSamples = converted * outChannels;
                int16_t *pcmPtr = reinterpret_cast<int16_t*>(outData[0]);

                pcm.reserve(pcm.size() + totalSamples);
                for (int i = 0; i < totalSamples; ++i) {
                    pcm.push_back(pcmPtr[i]);
                }

                // ---- Process FFT Chunks ----
                int consumed = 0;
                while ((pcm.size() - consumed) >= FFT_SIZE * outChannels) {
                    QVector<int16_t> mono(FFT_SIZE);

                    for (int i = 0; i < FFT_SIZE; ++i) {
                        mono[i] = pcm[consumed + i * outChannels];
                    }

                    processPcm(mono.data(), (qint64)FFT_SIZE);
                    consumed += FFT_SIZE * outChannels;

                    // ---- Playback Synchronization ----
                    // T_Budget ​= T_Sleep ​+ T_Process ​+ T_Communication
                    // T_Budget = FFT_SIZE / sampleRate
                    // e.g. T(s) = 1024 / 44100 ≈ 0.0232s ≈ 23.2ms
                    double duration_s = (double)FFT_SIZE / sampleRate; 
                    qint64 sleepMs = qRound(duration_s * 1000.0);
                    
                    // Sleeps 95% of the duration to account for processing overhead
                    QThread::msleep(qMax(1, qRound(sleepMs * 0.95)));

                    // Update offset after processing a chunk
                    currentSampleOffset += FFT_SIZE;
                }

                if (consumed > 0) pcm.erase(pcm.begin(), pcm.begin() + consumed);

                av_freep(&outData[0]);
                av_freep(&outData);
                av_frame_unref(frame);
            }

            av_packet_unref(pkt);
        } else break;
    }
}

void FFmpegAudioAnalyzer::processPcm(const int16_t *samples, qint64 count) {
    // ---- Hanning Window Function ----
    for (int i = 0; i < count; ++i) {
        double w = 0.5 * (1.0 - cos(2.0 * M_PI * i / FFT_SIZE));
        m_fftIn[i] = samples[i] * w;
    }

    fftw_execute(m_fftPlan);

    QVector<double> bands(12, 0.0);
    double bin = sampleRate / double(FFT_SIZE);

    const QVector<int> freqs = { 
        31, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 12000, 16000, 20000 
    };

    QVector<int> bandIndices(12);
    for (int i = 0; i < 12; ++i) {
        bandIndices[i] = qMin((int)(freqs[i] / bin), FFT_SIZE / 2);
    }
    
    int prevBin = 1;
    int maxBin  = FFT_SIZE / 2;

    auto energy = [&](int a, int b) {
        double e = 0;
        for (int i = a; i < b && i < maxBin; ++i) {
            e += m_fftOut[i][0] * m_fftOut[i][0] + m_fftOut[i][1] * m_fftOut[i][1];
        }
        return e;
    };

    for (int i = 0; i < 11; ++i) {
        int currentBin = bandIndices[i];
        if (currentBin > prevBin) bands[i] = energy(prevBin, currentBin); 
        else bands[i] = energy(prevBin, prevBin + 1);
        prevBin = currentBin;
    }
    bands[11] = energy(prevBin, maxBin);

    const double LOG_DIVISOR = 70.0;

    for (int i = 0; i < 12; ++i) {
        bands[i] = qBound(0.0, log(bands[i] + 1.0) / LOG_DIVISOR, 1.0);
    }

    emit spectraAvailable(bands);
}