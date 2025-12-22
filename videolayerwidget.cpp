#include "videolayerwidget.h"

VideoLayerWidget::VideoLayerWidget(QWidget *parent)
    : QWidget(parent) {
    setAutoFillBackground(false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
}

void VideoLayerWidget::setVideoFrame(const QVideoFrame &frame) {
    {
        QMutexLocker lock(&m_mutex);

        if (!frame.isValid() && !m_frame.isValid()) return;
        if (m_frame == frame) return;

        // m_frame = QVideoFrame(frame); // Deep copy
        m_frame = frame;
    }
    
    update();
}

void VideoLayerWidget::paintEvent(QPaintEvent *event) {
    QPainter p(this);

    QVideoFrame frameCopy;
    {
        QMutexLocker lock(&m_mutex);
        frameCopy = m_frame;
    }

    if (!frameCopy.isValid()) {
        p.fillRect(event->rect(), kBackgroundColor);
        return;
    }

    // ---- Letterbox ----
    QSize videoSize = frameCopy.size();
    QSize scaledSize = videoSize.scaled(size(), Qt::KeepAspectRatio);
    
    // Rect to draw the video
    QRect videoRect(
        (width() - scaledSize.width()) / 2,
        (height() - scaledSize.height()) / 2,
        scaledSize.width(),
        scaledSize.height()
    );

    // Black bars (pillarboxing)
    if (videoRect != rect()) {
        QRegion backgroundRegion(rect());
        backgroundRegion -= QRegion(videoRect);
        
        for (const QRect &r : backgroundRegion) {
            p.fillRect(r, kBackgroundColor);
        }
    }

    // ---- Drawing ----
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    
    QVideoFrame::PaintOptions options;
    options.backgroundColor = kBackgroundColor;
    options.aspectRatioMode = Qt::KeepAspectRatio;

    frameCopy.paint(&p, videoRect, options);
}