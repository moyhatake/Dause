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

    // ---- Styles ----
    static constexpr const char *MEDIA_STYLE = R"(
        QWidget {
            color: #99a0b2;
            background-color: #05080e;
        }

        QScrollArea, QFrame {
            padding: 1px;
            border-radius: 6px;
            border: 2px solid #99a0b2;
        }

        QLabel {
            border: none;
        }

        QToolTip {
            opacity: 128;
            padding: 0px 3px;
            color: #99a0b2;
            border-radius: 6px;
            border: 1px solid #99a0b2;
            background-color: #05080e;
        }

        QSlider::groove:horizontal {
            height: 5px; 
            border: none;
            border-radius: 2px;
            background: #05080e;
        }

        QSlider::handle:horizontal {
            width: 7px;
            height: 7px;
            margin: -3px 0;
            border-radius: 5px;
            background: #1f2e40;
            border: 2px solid #99a0b2;
        }

        QSlider::sub-page:horizontal {
            border: none;
            border-radius: 2px;
            background: #99a0b2;
        }

        QSlider::add-page:horizontal {
            border: none;
            border-radius: 2px;
            background: #1f2e40;
        }

        QSlider::handle:horizontal:hover {
            background: #99a0b2;
        }

        QScrollBar:vertical {
            width: 7px;
            border: none;
            border-radius: 3px;
            background: transparent;
        }
        QScrollBar::add-line:vertical, 
        QScrollBar::sub-line:vertical {
            height: 0px;
            border: none;
            background: none;
            subcontrol-position: top;
            subcontrol-origin: margin;
        }
        QScrollBar::up-arrow:vertical, 
        QScrollBar::down-arrow:vertical {
            width: 0px;
            height: 0px;
        }
        QScrollBar::handle:vertical {
            border: none;
            min-height: 20px;
            border-radius: 3px;
            background: #1f2e40;
        }
        QScrollBar::handle:vertical:hover {
            background: #99a0b2;
        }

        QScrollBar:horizontal {
            height: 7px;
            border: none;
            border-radius: 3px;
            background: transparent;
        }
        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal {
            width: 0px;
            border: none;
            background: none;
        }
        QScrollBar::left-arrow:horizontal,
        QScrollBar::right-arrow:horizontal {
            width: 0px;
            height: 0px;
        }
        QScrollBar::handle:horizontal {
            border: none;
            min-width: 20px;
            border-radius: 3px;
            background: #1f2e40;
        }
        QScrollBar::handle:horizontal:hover {
            background: #99a0b2;
        }

        QMenu {
            color: #99a0b2;
            border-radius: 6px;
            border: 1px solid #99a0b2;
            background-color: rgba(5, 8, 14, 128);
        }
        QMenu::indicator {
            width: 0px;
            height: 0px;
        }
        QMenu::item {
            left: -10px;
            height: 26px;
            padding: 0 6px;
            min-width: 78px;
            border-radius: 6px;
        }
        QMenu::item:selected {
            background-color: rgba(31, 46, 64, 128);
        }
        QMenu::item:checked {
            color: #05080e;
            background-color: rgba(153, 160, 178, 128);
        }
    )";
    static constexpr const char *COVER_STYLE = R"(
        QFrame {
            padding: 0;
            border: none;
            background-color: transparent;
        }
    )";
    static constexpr const char *SUBTITLE_STYLE = R"(
        QLabel {
            color: white;
            font-size: 28px;
            padding: 3px 9px;
            margin-bottom: 9px;
            border-radius: 6px;
            background: rgba(5, 8, 14, 128);
        }
    )";
    
private slots:
    void handleSubtitleData(const QString &subtitleText);
};

#endif // MEDIAWINDOW_H