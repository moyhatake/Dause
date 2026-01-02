#include "animatedbutton.h"
#include "ffmpegaudioanalyzer.h"
#include "marqueelabel.h"
#include "mediawindow.h"
#include "seekslider.h"

MediaWindow::MediaWindow(const QString &path, QWidget *parent)
    : QWidget(parent) {
    QFileInfo info(path);
    QString ext = info.suffix().toLower();
    setWindowTitle("Dause Media ~ " + info.fileName());
    setWindowIcon(QIcon(":/icons/assets/isotipo_solo.png"));
    setStyleSheet(MEDIA_STYLE);
    setWindowOpacity(0.75);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);

    if (isImage(ext) || isVideo(ext)) {
        resize(640, 360);
        setMinimumSize(640, 360);
        if (isImage(ext)) setupImage(path, layout);
        else setupVideo(path, layout);
    } else if (isAudio(ext)) {
        resize(360, 360);
        setMinimumSize(360, 360);
        setMaximumSize(360, 360);
        setupAudio(path, layout);
    }
}

MediaWindow::~MediaWindow() {
    if (player) player->stop();
}

bool MediaWindow::isImage(const QString &ext) const {
    return QStringList{"png","jpg","jpeg","webp","gif"}.contains(ext);
}
bool MediaWindow::isAudio(const QString &ext) const {
    return QStringList{"mp3","m4a","flac","wav","ogg"}.contains(ext);
}
bool MediaWindow::isVideo(const QString &ext) const {
    return QStringList{"mp4","mkv","avi","mov","flv"}.contains(ext);
}

void MediaWindow::mousePressEvent(QMouseEvent *event) {
    if (currentZoom == 2.0 && event->button() == Qt::LeftButton) {
        lastDragPos = event->pos();
        isDragging = true;
        
        setCursor(Qt::SizeAllCursor);
        
        event->accept();
    } else QWidget::mousePressEvent(event);
}

void MediaWindow::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPoint delta = event->pos() - lastDragPos;
        
        QScrollBar *hBar = scrollArea->horizontalScrollBar();
        QScrollBar *vBar = scrollArea->verticalScrollBar();
        
        hBar->setValue(hBar->value() - delta.x());
        vBar->setValue(vBar->value() - delta.y());
        
        lastDragPos = event->pos();
        
        event->accept();
    } else QWidget::mouseMoveEvent(event);
}

void MediaWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (isDragging && event->button() == Qt::LeftButton) {
        isDragging = false;
        unsetCursor();
        
        event->accept();
    } else QWidget::mouseReleaseEvent(event);
}

void MediaWindow::resizeEvent(QResizeEvent *event) {
    if (currentZoom == 1.0) setZoom();
    
    QWidget::resizeEvent(event);
}

bool MediaWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        
        if (mouseEvent->button() == Qt::LeftButton) {
            if (watched == img) {
                if (currentZoom == 1.0) {
                    currentZoom = 2.0;

                    scrollArea->setWidgetResizable(false);
                    img->setPixmap(px.scaled(originalImageSize * currentZoom, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    img->adjustSize();

                    setMinimumSize(0,0);
                    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                } else {
                    currentZoom = 1.0;

                    scrollArea->setWidgetResizable(true);
                    setZoom();

                    setMinimumSize(0,0);
                    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                }
            } else if (watched == video) {
                if (windowState() & Qt::WindowMaximized) showNormal();
                else showMaximized();
            } else {
                isVisualizerActive = !isVisualizerActive;
                cover->setVisible(!isVisualizerActive);
                visualizer->setVisible(isVisualizerActive);
            }

            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonRelease && watched == video) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::RightButton) {
            inflateStreamMenu();
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void MediaWindow::keyPressEvent(QKeyEvent *event) {
    AnimatedButton *targetBtn = nullptr;

    switch (event->key()) {
        case Qt::Key_Space:
            targetBtn = playPause;
            break;
        case Qt::Key_Left:
            targetBtn = back10;
            break;
        case Qt::Key_Right:
            targetBtn = forward10;
            break;
        case Qt::Key_M:
            targetBtn = mute;
            break;
        case Qt::Key_F:
            targetBtn = focus;
            break;
        default:
            break;
    }

    if (targetBtn && targetBtn->isEnabled()) {
        targetBtn->click();
        event->accept();
        return;
    }
    
    QWidget::keyPressEvent(event);
}

void MediaWindow::setZoom() {
    if (!img || px.isNull()) return;

    if (currentZoom == 1.0) {
        QSize targetSize = calcImgSize(size()); 
        img->setPixmap(px.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        img->resize(targetSize);
    }
}

QSize MediaWindow::calcImgSize(const QSize& viewportSize) const {
    if (originalImageSize.width() <= viewportSize.width() && 
        originalImageSize.height() <= viewportSize.height()) {
        return originalImageSize;
    } else
        return px.scaled(viewportSize, Qt::KeepAspectRatio, Qt::SmoothTransformation).size();
}

void MediaWindow::handleAudioTrackSelection(int index) {
    player->setActiveAudioTrack(index);
    currentAudioTrackIndex = index;
}

void MediaWindow::centerMenu(QMenu *menu) {
    QPoint center = vidSubContainer->mapToGlobal(vidSubContainer->rect().center());
    QPoint pos = center - QPoint(menu->sizeHint().width() / 2, menu->sizeHint().height() / 2);
    menu->popup(pos);
}

void MediaWindow::inflateStreamMenu() {
    if (!streamMenu) {
        streamMenu = new QMenu(this);
        streamMenu->setAttribute(Qt::WA_TranslucentBackground);
    }
    streamMenu->clear();
    
    availableQtAudioTracks = player->audioTracks();
    QList<FFmpegSubtitleAnalyzer::SubtitleTrack> subtitleTracks = subtitleAnalyzer->availableTracks;

    // ---- Audio Stream ----
    if (availableQtAudioTracks.size() > 1) {
        QMenu *audioMenu = streamMenu->addMenu("Audio");
        audioMenu->setAttribute(Qt::WA_TranslucentBackground);
        
        for (int i = 0; i < availableQtAudioTracks.size(); ++i) {
            QString name = availableQtAudioTracks[i].value(QMediaMetaData::Language).toString().toUpper();
            if (name.isEmpty() || name == "UND") name = QString("Track %1").arg(i + 1);

            QAction *action = audioMenu->addAction(name);
            action->setData(i);
            action->setCheckable(true);

            if (i == currentAudioTrackIndex) action->setChecked(true);
        }

        connect(audioMenu, &QMenu::triggered, this, [this](QAction *action) {
            int index = action->data().toInt();
            handleAudioTrackSelection(index);
        });
    }

    // ---- Subtitle Stream ----
    if (!subtitleTracks.isEmpty()) { 
        QMenu *subtitleMenu = streamMenu->addMenu("Subtitle");
        subtitleMenu->setAttribute(Qt::WA_TranslucentBackground);
        
        QAction *offAction = subtitleMenu->addAction("Off");
        offAction->setData(-1);
        offAction->setCheckable(true);
        
        if (currentSubtitleTrackIndex == -1) offAction->setChecked(true);
        
        for (const auto& track : subtitleTracks) {
            QString name = track.language.toUpper();
            if (name.isEmpty() || name == "UND") name = QString("Track %1").arg(track.streamIndex + 1);

            QAction *action = subtitleMenu->addAction(name);
            action->setData(track.streamIndex);
            action->setCheckable(true);
            
            if (track.streamIndex == currentSubtitleTrackIndex) action->setChecked(true);
        }
        
        connect(subtitleMenu, &QMenu::triggered, this, [this](QAction *action) {
            int index = action->data().toInt();
            currentSubtitleTrackIndex = index;
            subtitleAnalyzer->selectTrack(index);
        });
    }

    if (!streamMenu->isEmpty()) centerMenu(streamMenu);
}

void MediaWindow::handleSubtitleData(const QString &subtitleText) {
    if (subtitleText.isEmpty()) subtitleLabel->hide();
    else {
        subtitleLabel->setText(subtitleText);
        subtitleLabel->adjustSize();
        subtitleLabel->show();
        subtitleLabel->raise();
    }
}

void MediaWindow::setupImage(const QString &path, QVBoxLayout *layout) {
    img = new QLabel;
    px.load(path);

    originalImageSize = px.size();

    img->setAlignment(Qt::AlignCenter);
    img->setScaledContents(false);
    img->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setWidget(img);

    layout->addWidget(scrollArea);
    
    setZoom();

    img->installEventFilter(this);
}

void MediaWindow::setupAudio(const QString &path, QVBoxLayout *layout) {
    player = new QMediaPlayer(this);
    audio  = new QAudioOutput(this);
    player->setAudioOutput(audio);
    audio->setVolume(1.0);

    QFrame *audioFrame = new QFrame(this);
    audioFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *frameLayout = new QVBoxLayout(audioFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0); 
    frameLayout->setSpacing(0);

    // ---- Audio Analizer & Spectrum Visualizer ----
    FFmpegAudioAnalyzer *analyzer = new FFmpegAudioAnalyzer(path, this);
    analyzer->start();

    visualizer = new SpectrumWidget(this);
    visualizer->setFixedSize(224, 224);
    visualizer->setVisible(false);

    // ---- Cover ----
    cover = new RoundedImageLabel(this);
    cover->setAlignment(Qt::AlignCenter);
    cover->setFixedSize(224, 224);

    QPixmap fallback(":/icons/assets/audio.svg");
    cover->setPixmap(fallback.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QFrame *coverFrame = new QFrame(this);
    coverFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    coverFrame->setStyleSheet(COVER_STYLE);

    QHBoxLayout *coverLayout = new QHBoxLayout(coverFrame);
    coverLayout->setSpacing(0);
    coverLayout->setContentsMargins(0, 0, 0, 0);
    coverLayout->setAlignment(Qt::AlignCenter);
    coverLayout->addStretch();
    coverLayout->addWidget(cover);
    coverLayout->addWidget(visualizer);
    coverLayout->addStretch();

    // ---- Info ----
    MarqueeLabel *titleLabel = new MarqueeLabel(this);
    titleLabel->setMarqueeEnabled(true);
    QFont fontL = titleLabel->font();
    fontL.setBold(true);
    fontL.setPointSize(12);
    titleLabel->setFont(fontL);
    MarqueeLabel *artistLabel = new MarqueeLabel(this);
    artistLabel->setMarqueeEnabled(true);
    QFont fontA = artistLabel->font();
    fontA.setBold(true);
    artistLabel->setFont(fontA);
    MarqueeLabel *albumLabel = new MarqueeLabel(this);
    albumLabel->setMarqueeEnabled(true);

    // ---- Media Controls ----
    playPause = new AnimatedButton(this);
    playPause->setToolTip("Pause");
    playPause->setMinimumSize(35, 28);
    playPause->setMaximumSize(35, 28);
    playPause->setIcon(QIcon(":/icons/assets/play.svg"));
    playPause->setFocusPolicy(Qt::NoFocus);

    back10 = new AnimatedButton(this);
    back10->setToolTip("10s");
    back10->setMinimumSize(35, 28);
    back10->setMaximumSize(35, 28);
    back10->setIcon(QIcon(":/icons/assets/backward.svg"));
    back10->setFocusPolicy(Qt::NoFocus);
    back10->setAutoRepeat(true);
    back10->setAutoRepeatDelay(500);
    back10->setAutoRepeatInterval(50);
    
    forward10 = new AnimatedButton(this);
    forward10->setToolTip("10s");
    forward10->setMinimumSize(35, 28);
    forward10->setMaximumSize(35, 28);
    forward10->setIcon(QIcon(":/icons/assets/forward.svg"));
    forward10->setFocusPolicy(Qt::NoFocus);
    forward10->setAutoRepeat(true);
    forward10->setAutoRepeatDelay(500);
    forward10->setAutoRepeatInterval(50);

    // ---- Position/Seek Controls ----
    timeDisplayLabel = new QLabel("0:00 / 0:00");
    timeDisplayLabel->setAlignment(Qt::AlignCenter);

    SeekSlider *slider = new SeekSlider(Qt::Horizontal);
    slider->setRange(0, 1000);
    slider->setFocusPolicy(Qt::NoFocus);

    // ---- Volume Controls ----
    QLabel *volumePercentLabel = new QLabel("100%");
    volumePercentLabel->setAlignment(Qt::AlignCenter);

    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(100);
    volumeSlider->setMinimumWidth(70);
    volumeSlider->setMaximumWidth(70);
    volumeSlider->setFocusPolicy(Qt::NoFocus);

    mute = new AnimatedButton(this);
    mute->setToolTip("Mute");
    mute->setMinimumSize(35, 28);
    mute->setMaximumSize(35, 28);
    mute->setIcon(QIcon(":/icons/assets/volume_full.svg"));
    mute->setFocusPolicy(Qt::NoFocus);

    auto seekAnalyzer = [=](qint64 positionMs) {
        analyzer->seekTo(positionMs);
        analyzer->toggleRunning(true);
    };

    connect(analyzer, &FFmpegAudioAnalyzer::spectraAvailable, visualizer, &SpectrumWidget::updateSpectra, Qt::QueuedConnection);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            QMediaMetaData metaData = player->metaData();

            // ---- Cover Image ----
            QVariant data;
            data = metaData.value(QMediaMetaData::CoverArtImage);
            if (!data.isValid() || !data.canConvert<QPixmap>())
                data = metaData.value(QMediaMetaData::ThumbnailImage);

            if (data.isValid() && data.canConvert<QPixmap>()) {
                QPixmap coverImg = data.value<QPixmap>();
                cover->setPixmap(coverImg);
            }

            // ---- Title ----
            QString title = metaData.value(QMediaMetaData::Title).toString();
            if (title.isEmpty()) {
                title = QUrl(player->source()).fileName();
                if (title.contains('.'))
                    title.truncate(title.lastIndexOf('.'));
            }
            titleLabel->setText(title);

            // ---- Artist ----
            QString artist = metaData.value(QMediaMetaData::AlbumArtist).toString();
            if (artist.isEmpty())
                artist = metaData.value(QMediaMetaData::ContributingArtist).toString();
            artistLabel->setText(artist); 

            // ---- Album ----
            QString album = metaData.value(QMediaMetaData::AlbumTitle).toString();
            albumLabel->setText(album);
            
            artistLabel->setVisible(!artist.isEmpty());
            albumLabel->setVisible(!album.isEmpty());
        }
    });

    connect(playPause, &AnimatedButton::clicked, this, [=] {
        QIcon icon;

        if (player->playbackState() == QMediaPlayer::PlayingState) {
            player->pause();
            icon = QIcon(":/icons/assets/pause.svg");
            playPause->setToolTip("Play");
            wasPlayingBeforeSeek = false;
        } else {
            player->play();
            icon = QIcon(":/icons/assets/play.svg");
            playPause->setToolTip("Pause");
            wasPlayingBeforeSeek = true;
        }

        playPause->fadeIconSwap(icon);
        playPause->animateCapsState(false);
    });

    connect(back10, &AnimatedButton::clicked, this, [=] {
        player->setPosition(player->position() - 10000);
    });

    connect(forward10, &AnimatedButton::clicked, this, [=] {
        player->setPosition(player->position() + 10000);
    });

    connect(mute, &AnimatedButton::clicked, this, [=] {
        audio->setMuted(!audio->isMuted()); 

        QIcon icon;
        if (audio->isMuted()) {
            icon = QIcon(":/icons/assets/mute.svg");
            mute->setToolTip("Unmute");
        } else {
            double currentVolume = audio->volume() * 100;
            if (currentVolume > 66) icon = QIcon(":/icons/assets/volume_full.svg");
            else if (currentVolume > 33) icon = QIcon(":/icons/assets/volume_half.svg");
            else icon = QIcon(":/icons/assets/volume_none.svg");
            mute->setToolTip("Mute");
        }

        mute->fadeIconSwap(icon);
        mute->animateCapsState(false);
    });

    connect(volumeSlider, &QSlider::sliderMoved, this, [=](int value) {
        audio->setVolume(value / 100.0);
        volumePercentLabel->setText(QString::number(value) + "%");
        
        QIcon icon;
        if (value > 66) icon = QIcon(":/icons/assets/volume_full.svg");
        else if (value > 33) icon = QIcon(":/icons/assets/volume_half.svg");
        else icon = QIcon(":/icons/assets/volume_none.svg");

        mute->setIcon(icon);
    });

    connect(slider, &SeekSlider::sliderPressed, this, [=] {
        player->pause();
    });

    connect(slider, &SeekSlider::sliderReleased, this, [=] {
        if (player->duration() > 0) {
            qint64 newPos = qint64(double(slider->value()) / 1000.0 * player->duration());
            player->setPosition(newPos);
            
            analyzer->seekTo(newPos); 
            analyzer->toggleRunning(true);
        }

        if (wasPlayingBeforeSeek) player->play();
    });
    
    connect(slider, &SeekSlider::sliderMoved, this, [=](int value) {
        if (player->duration() > 0) {
            qint64 tempPos = qint64(double(value) / 1000.0 * player->duration());
            
            QString currentTimeStr = QTime(0, 0).addMSecs(tempPos).toString(timeFormat);
            timeDisplayLabel->setText(QString("%1 / %2").arg(currentTimeStr).arg(totalTimeStr));
        }
    });

    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 dur) {
        if (dur >= 3600000) timeFormat = "h:mm:ss";
        else if (dur >= 600000) timeFormat = "mm:ss";
        else timeFormat = "m:ss";

        totalTimeStr = QTime(0, 0).addMSecs(dur).toString(timeFormat);
        timeDisplayLabel->setText(QString("0:00 / %1").arg(totalTimeStr));
    });

    connect(player, &QMediaPlayer::positionChanged, this, [=](qint64 pos) {
        analyzer->seekTo(pos);

        if (player->duration() > 0) {
            if (!slider->isSliderDown())
                slider->setValue(int(double(pos) / player->duration() * 1000));

            QString currentTimeStr = QTime(0, 0).addMSecs(pos).toString(timeFormat);
            timeDisplayLabel->setText(QString("%1 / %2").arg(currentTimeStr).arg(totalTimeStr));
        }
    });

    connect(player, &QMediaPlayer::playbackStateChanged, this, [=](QMediaPlayer::PlaybackState state) {
        if (state == QMediaPlayer::StoppedState) {
            QIcon playIcon(":/icons/assets/pause.svg");
            playPause->setIcon(playIcon);
            playPause->setToolTip("Play");
        }
	
	    bool running = (state == QMediaPlayer::PlayingState);
        analyzer->toggleRunning(running);
    });

    QHBoxLayout *timeline = new QHBoxLayout;
    timeline->setSpacing(9);
    timeline->setContentsMargins(6, 0, 8, 0);
    timeline->addWidget(timeDisplayLabel);
    timeline->addWidget(slider);

    QHBoxLayout *controls = new QHBoxLayout;
    controls->setSpacing(9);
    controls->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *mediaBtns = new QHBoxLayout;
    mediaBtns->setSpacing(0);
    mediaBtns->setContentsMargins(0, 0, 0, 0);
    mediaBtns->addWidget(back10);
    mediaBtns->addWidget(playPause);
    mediaBtns->addWidget(forward10);
    controls->addLayout(mediaBtns);
    controls->addStretch();

    controls->addWidget(volumePercentLabel);
    controls->addWidget(volumeSlider);
    controls->addWidget(mute);

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->setSpacing(0);
    infoLayout->setContentsMargins(6, 0, 6, 0); 
    infoLayout->addWidget(titleLabel);
    infoLayout->addWidget(artistLabel);
    infoLayout->addWidget(albumLabel);

    frameLayout->addWidget(coverFrame);
    frameLayout->addLayout(infoLayout);
    frameLayout->addLayout(timeline);
    frameLayout->addLayout(controls);
    layout->addWidget(audioFrame);

    player->setSource(QUrl::fromLocalFile(path));
    player->play();

    cover->installEventFilter(this);
    visualizer->installEventFilter(this);
}

void MediaWindow::setupVideo(const QString &path, QVBoxLayout *layout) {
    player = new QMediaPlayer(this);
    audio  = new QAudioOutput(this);
    player->setAudioOutput(audio);
    audio->setVolume(1.0);

    QFrame *videoFrame = new QFrame(this);
    videoFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QGridLayout *gridLayout = new QGridLayout(videoFrame);
    gridLayout->setContentsMargins(0, 0, 0, 0); 
    gridLayout->setSpacing(0);

    // ---- Video/Subtitle Container ----
    vidSubContainer = new QWidget(videoFrame);
    vidSubContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vidSubContainer->setAttribute(Qt::WA_NoSystemBackground);

    QGridLayout *overlayLayout = new QGridLayout(vidSubContainer);
    overlayLayout->setContentsMargins(0, 0, 0, 0);

    // ---- Video Player ----
    video = new VideoLayerWidget(vidSubContainer);
    video->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVideoSink *videoSink = new QVideoSink(this);
    player->setVideoSink(videoSink);

    // ---- Subtitle Analyzer ----
    subtitleAnalyzer = new FFmpegSubtitleAnalyzer(path, this);
    connect(subtitleAnalyzer, &FFmpegSubtitleAnalyzer::subtitleDataAvailable, this, &MediaWindow::handleSubtitleData);
    subtitleAnalyzer->start();

    // ---- Subtitle Label ----
    subtitleLabel = new QLabel(vidSubContainer);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet(SUBTITLE_STYLE);
    subtitleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    subtitleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    subtitleLabel->hide();

    // ---- Media Controls ----
    playPause = new AnimatedButton(this);
    playPause->setToolTip("Pause");
    playPause->setMinimumSize(35, 28);
    playPause->setMaximumSize(35, 28);
    playPause->setIcon(QIcon(":/icons/assets/play.svg"));
    playPause->setFocusPolicy(Qt::NoFocus);

    back10 = new AnimatedButton(this);
    back10->setToolTip("10s");
    back10->setMinimumSize(35, 28);
    back10->setMaximumSize(35, 28);
    back10->setIcon(QIcon(":/icons/assets/backward.svg"));
    back10->setFocusPolicy(Qt::NoFocus);
    back10->setAutoRepeat(true);
    back10->setAutoRepeatDelay(500);
    back10->setAutoRepeatInterval(50);
    
    forward10 = new AnimatedButton(this);
    forward10->setToolTip("10s");
    forward10->setMinimumSize(35, 28);
    forward10->setMaximumSize(35, 28);
    forward10->setIcon(QIcon(":/icons/assets/forward.svg"));
    forward10->setFocusPolicy(Qt::NoFocus);
    forward10->setAutoRepeat(true);
    forward10->setAutoRepeatDelay(500);
    forward10->setAutoRepeatInterval(50);

    // ---- Position/Seek Controls ----
    timeDisplayLabel = new QLabel("0:00 / 0:00");
    timeDisplayLabel->setAlignment(Qt::AlignCenter);

    SeekSlider *slider = new SeekSlider(Qt::Horizontal);
    slider->setRange(0, 1000);
    slider->setFocusPolicy(Qt::NoFocus);

    // ---- Volume Controls ----
    QLabel *volumePercentLabel = new QLabel("100%");
    volumePercentLabel->setAlignment(Qt::AlignCenter);

    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(100);
    volumeSlider->setMinimumWidth(70);
    volumeSlider->setMaximumWidth(70);
    volumeSlider->setFocusPolicy(Qt::NoFocus);

    mute = new AnimatedButton(this);
    mute->setToolTip("Mute");
    mute->setMinimumSize(35, 28);
    mute->setMaximumSize(35, 28);
    mute->setIcon(QIcon(":/icons/assets/volume_full.svg"));
    mute->setFocusPolicy(Qt::NoFocus);

    focus = new AnimatedButton(this);
    focus->setToolTip("Focus");
    focus->setMinimumSize(35, 28);
    focus->setMaximumSize(35, 28);
    focus->setIcon(QIcon(":/icons/assets/unfocus.svg"));
    focus->setFocusPolicy(Qt::NoFocus);

    connect(videoSink, &QVideoSink::videoFrameChanged, video, &VideoLayerWidget::setVideoFrame);

    connect(playPause, &AnimatedButton::clicked, this, [=] {
        QIcon icon;

        if (player->playbackState() == QMediaPlayer::PlayingState) {
            player->pause();
            icon = QIcon(":/icons/assets/pause.svg");
            playPause->setToolTip("Play");
            wasPlayingBeforeSeek = false;
        } else {
            player->play();
            icon = QIcon(":/icons/assets/play.svg");
            playPause->setToolTip("Pause");
            wasPlayingBeforeSeek = true;
        }

        playPause->fadeIconSwap(icon);
        playPause->animateCapsState(false);
    });

    connect(back10, &AnimatedButton::clicked, this, [=] {
        player->setPosition(player->position() - 10000);
    });

    connect(forward10, &AnimatedButton::clicked, this, [=] {
        player->setPosition(player->position() + 10000);
    });

    connect(mute, &AnimatedButton::clicked, this, [=] {
        audio->setMuted(!audio->isMuted()); 

        QIcon icon;
        if (audio->isMuted()) {
            icon = QIcon(":/icons/assets/mute.svg");
            mute->setToolTip("Unmute");
        } else {
            double currentVolume = audio->volume() * 100;
            if (currentVolume > 66) icon = QIcon(":/icons/assets/volume_full.svg");
            else if (currentVolume > 33) icon = QIcon(":/icons/assets/volume_half.svg");
            else icon = QIcon(":/icons/assets/volume_none.svg");
            mute->setToolTip("Mute");
        }

        mute->fadeIconSwap(icon);
        mute->animateCapsState(false);
    });

    connect(focus, &AnimatedButton::clicked, this, [=] {
        isVideoFocus = !isVideoFocus;

        QIcon icon;
        if (isVideoFocus) {
            setWindowOpacity(1);
            icon = QIcon(":/icons/assets/focus.svg");
            focus->setToolTip("Unfocus");
        } else {
            setWindowOpacity(0.75);
            icon = QIcon(":/icons/assets/unfocus.svg");
            focus->setToolTip("Focus");
        }

        focus->fadeIconSwap(icon);
        focus->animateCapsState(false);
    });

    connect(volumeSlider, &QSlider::sliderMoved, this, [=](int value) {
        audio->setVolume(value / 100.0);
        volumePercentLabel->setText(QString::number(value) + "%");
        
        QIcon icon;
        if (value > 66) icon = QIcon(":/icons/assets/volume_full.svg");
        else if (value > 33) icon = QIcon(":/icons/assets/volume_half.svg");
        else icon = QIcon(":/icons/assets/volume_none.svg");

        mute->setIcon(icon);
    });

    connect(slider, &SeekSlider::sliderPressed, this, [=] {
        player->pause();
    });

    connect(slider, &SeekSlider::sliderReleased, this, [=] {
        if (player->duration() > 0) {
            qint64 newPos = qint64(double(slider->value()) / 1000.0 * player->duration());
            
            player->setPosition(newPos);
            subtitleAnalyzer->seekTo(newPos);
        }

        if (wasPlayingBeforeSeek) player->play();
    });
    
    connect(slider, &SeekSlider::sliderMoved, this, [=](int value) {
        if (player->duration() > 0) {
            qint64 tempPos = qint64(double(value) / 1000.0 * player->duration());
            QString currentTimeStr = QTime(0, 0).addMSecs(tempPos).toString(timeFormat);
            timeDisplayLabel->setText(QString("%1 / %2").arg(currentTimeStr).arg(totalTimeStr));
        }
    });

    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 dur) {
        if (dur >= 3600000) timeFormat = "h:mm:ss";
        else if (dur >= 600000) timeFormat = "mm:ss";
        else timeFormat = "m:ss";

        totalTimeStr = QTime(0, 0).addMSecs(dur).toString(timeFormat);
        timeDisplayLabel->setText(QString("0:00 / %1").arg(totalTimeStr));
    });

    connect(player, &QMediaPlayer::positionChanged, this, [=](qint64 pos) {
        if (player->duration() > 0) {
            if (!slider->isSliderDown())
                slider->setValue(int(double(pos) / player->duration() * 1000));

            QString currentTimeStr = QTime(0, 0).addMSecs(pos).toString(timeFormat);
            timeDisplayLabel->setText(QString("%1 / %2").arg(currentTimeStr).arg(totalTimeStr));
        }
    });

    connect(player, &QMediaPlayer::positionChanged, subtitleAnalyzer, &FFmpegSubtitleAnalyzer::seekTo);

    connect(player, &QMediaPlayer::playbackStateChanged, this, [=](QMediaPlayer::PlaybackState state) {
        if (state == QMediaPlayer::StoppedState) {
            QIcon playIcon(":/icons/assets/pause.svg");
            playPause->setIcon(playIcon);
            playPause->setToolTip("Play");
        }

        bool running = (state == QMediaPlayer::PlayingState);
        subtitleAnalyzer->toggleRunning(running);
    });

    QHBoxLayout *controls = new QHBoxLayout;
    controls->setSpacing(9);
    controls->setContentsMargins(0, 0, 0, 0);
    
    QHBoxLayout *mediaBtns = new QHBoxLayout;
    mediaBtns->setSpacing(0);
    mediaBtns->setContentsMargins(0, 0, 0, 0);
    mediaBtns->addWidget(back10);
    mediaBtns->addWidget(playPause);
    mediaBtns->addWidget(forward10);
    controls->addLayout(mediaBtns);

    controls->addWidget(timeDisplayLabel);
    controls->addWidget(slider);

    controls->addWidget(volumePercentLabel);
    controls->addWidget(volumeSlider);
    controls->addWidget(mute);
    controls->addWidget(focus);

    overlayLayout->addWidget(video, 0, 0);
    overlayLayout->addWidget(subtitleLabel, 0, 0, Qt::AlignHCenter | Qt::AlignBottom);

    gridLayout->addWidget(vidSubContainer, 0, 0);
    gridLayout->addLayout(controls, 1, 0);

    layout->addWidget(videoFrame);

    player->setSource(QUrl::fromLocalFile(path));
    player->play();

    video->installEventFilter(this);
}