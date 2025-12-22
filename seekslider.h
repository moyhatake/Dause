#ifndef SEEKSLIDER_H
#define SEEKSLIDER_H

#include <QStyle>
#include <QWidget>
#include <QSlider>
#include <QMouseEvent>
#include <QStyleOptionSlider>

class SeekSlider : public QSlider {
    Q_OBJECT

public:
    explicit SeekSlider(QWidget *parent = nullptr);
    explicit SeekSlider(Qt::Orientation orientation, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // SEEKSLIDER_H