#ifndef VIDEOLAYERWIDGET_H
#define VIDEOLAYERWIDGET_H

#include <QColor>
#include <QMutex>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QVideoFrame>
#include <QMutexLocker>

class VideoLayerWidget : public QWidget {
    Q_OBJECT

public:
    explicit VideoLayerWidget(QWidget *parent = nullptr);
    void setVideoFrame(const QVideoFrame &frame);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMutex m_mutex;
    QVideoFrame m_frame;

    const QColor kBackgroundColor = QColor("#05080e");
};

#endif // VIDEOLAYERWIDGET_H