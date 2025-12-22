#ifndef FFMPEGAUDIOANALYZER_H
#define FFMPEGAUDIOANALYZER_H

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

extern "C" {
    #include <libavutil/avutil.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/samplefmt.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libavutil/channel_layout.h>
}

#include <QtMath>
#include <QMutex>
#include <QObject>
#include <QVector>
#include <QThread>
#include <fftw3.h>

class FFmpegAudioAnalyzer : public QThread {
    Q_OBJECT

public:
    explicit FFmpegAudioAnalyzer(const QString &path, QObject *parent = nullptr);
    ~FFmpegAudioAnalyzer();

public slots:
    void seekTo(qint64 positionMs);
    void toggleRunning(bool running);

signals:
    void spectraAvailable(const QVector<double>& bands);

protected:
    void run() override;

private:
    QString filePath;
    int sampleRate = 44100;               // Default/Fallback Sample Rate
    static constexpr int FFT_SIZE = 1024; // 2¹⁰

    QMutex seekMutex;
    std::atomic<bool> isRunning = true;
    std::atomic<qint64> targetPositionMs = 0;

    double *m_fftIn;
    fftw_complex *m_fftOut;
    fftw_plan m_fftPlan;

    void processPcm(const int16_t *samples, qint64 sampleCount);
};

#endif // FFMPEGAUDIOANALYZER_H