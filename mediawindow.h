#ifndef MEDIAWINDOW_H
#define MEDIAWINDOW_H

#include <QUrl>
#include <QIcon>
#include <QTime>
#include <QMenu>
#include <QList>
#include <QLabel>
#include <QPoint>
#include <QString>
#include <QWidget>
#include <QSlider>
#include <QPixmap>
#include <QVariant>
#include <QFileInfo>
#include <QScrollBar>
#include <QVideoSink>
#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMouseEvent>
#include <QScrollArea>
#include <QResizeEvent>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include "spectrumwidget.h"
#include "videolayerwidget.h"
#include "roundedimagelabel.h"
#include "ffmpegsubtitleanalyzer.h"

class QMenu;
class QLabel;
class FFmpegSubtitleAnalyzer;

class MediaWindow : public QWidget {
    Q_OBJECT

public:
    explicit MediaWindow(const QString &path, QWidget *parent = nullptr);
    ~MediaWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    FFmpegSubtitleAnalyzer *subtitleAnalyzer = nullptr;
    QList<QMediaMetaData> availableQtAudioTracks;
    SpectrumWidget *visualizer = nullptr;
    AnimatedButton *playPause = nullptr;
    AnimatedButton *forward10 = nullptr;
    QWidget *vidSubContainer = nullptr;
    QLabel *timeDisplayLabel = nullptr;
    RoundedImageLabel *cover = nullptr;
    VideoLayerWidget *video = nullptr;
    QScrollArea *scrollArea = nullptr;
    AnimatedButton *back10 = nullptr;
    QLabel *subtitleLabel = nullptr;
    AnimatedButton *focus = nullptr;
    AnimatedButton *mute = nullptr;
    QMediaPlayer *player = nullptr;
    QAudioOutput *audio = nullptr;
    QMenu *streamMenu = nullptr;
    QLabel *img = nullptr;

    QString totalTimeStr = "0:00";
    QString timeFormat = "m:ss";
    qreal currentZoom = 1.0;
    QSize originalImageSize;
    QPoint lastDragPos;
    QPixmap px;

    int currentSubtitleTrackIndex = -1;
    int currentAudioTrackIndex = 0;
    
    bool isImage(const QString &ext) const;
    bool isAudio(const QString &ext) const;
    bool isVideo(const QString &ext) const;
    bool wasPlayingBeforeSeek = true;
    bool isVisualizerActive = false;
    bool isVideoFocus = false;
    bool isDragging = false;
    
    void setupImage(const QString &path, QVBoxLayout *layout);
    void setupAudio(const QString &path, QVBoxLayout *layout);
    void setupVideo(const QString &path, QVBoxLayout *layout);

    QSize calcImgSize(const QSize& viewportSize) const;
    void setZoom();

    void handleAudioTrackSelection(int index);
    void centerMenu(QMenu *menu);
    void inflateStreamMenu();

private slots:
    void handleSubtitleData(const QString &subtitleText);
};

#endif // MEDIAWINDOW_H