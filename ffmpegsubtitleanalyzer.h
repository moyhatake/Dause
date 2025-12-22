#ifndef FFMPEGSUBTITLEANALYZER_H
#define FFMPEGSUBTITLEANALYZER_H

#include <QMutex>
#include <QThread>
#include <QVector>
#include <QAtomicInt>
#include <QRegularExpression>

extern "C" {
    #include <libavutil/time.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/mathematics.h>
}

class FFmpegSubtitleAnalyzer : public QThread {
    Q_OBJECT

public:
    explicit FFmpegSubtitleAnalyzer(const QString &path, QObject *parent = nullptr);
    ~FFmpegSubtitleAnalyzer();
    
    struct SubtitleTrack {
        int streamIndex;
        QString language;
    };
    QList<SubtitleTrack> availableTracks;

public slots:
    void seekTo(qint64 positionMs);
    void toggleRunning(bool running);
    void selectTrack(int streamIndex);

signals:
    void subtitleDataAvailable(const QString &subtitleText);

protected:
    void run() override;

private:
    QMutex seekMutex;
    QString filePath;
    std::atomic<bool> isRunning = false;
    std::atomic<qint64> targetPositionMs = 0;
    std::atomic<int> selectedStreamIndex = -1;

    void findSubtitleStreams(AVFormatContext *fmt);
};

#endif // FFMPEGSUBTITLEANALYZER_H