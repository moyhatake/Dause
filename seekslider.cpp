#include "seekslider.h"

SeekSlider::SeekSlider(QWidget *parent)
    : QSlider(parent) {
    setOrientation(Qt::Horizontal);
}

SeekSlider::SeekSlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent) {}

void SeekSlider::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        
        if (orientation() == Qt::Horizontal) {
            int width = opt.rect.width();
            int newX = event->pos().x();
            int newValue = QStyle::sliderValueFromPosition(minimum(), maximum(), newX, width, false);
            
            setValue(newValue);
            
            emit sliderReleased();
        }
    }
    
    QSlider::mousePressEvent(event);
}