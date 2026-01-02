#include <QPainter>
#include <QResizeEvent>
#include "marqueelabel.h"

MarqueeLabel::MarqueeLabel(QWidget *parent)
    : QLabel(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MarqueeLabel::scrollText);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    QLabel::setText(QString());
}

void MarqueeLabel::setText(const QString &text) {
    QLabel::setText(text);
    updateMetrics();
    updateMarqueeState();
    update();
}

void MarqueeLabel::setMarqueeEnabled(bool enabled) {
    marqueeEnabled = enabled;
    updateMarqueeState();
}

void MarqueeLabel::updateMetrics() {
    QFontMetrics metrics(font());
    textWidth = metrics.horizontalAdvance(text());
    scrollOffset = 0;
}

void MarqueeLabel::updateMarqueeState() {
    if (!marqueeEnabled) {
        timer->stop();
        return;
    }
    
    if (textWidth > (width() - 16) && !text().isEmpty())
        QTimer::singleShot(3000, this, [this]{ timer->start(50); });
    else timer->stop();
}

void MarqueeLabel::resizeEvent(QResizeEvent *event) {
    QLabel::resizeEvent(event);
    updateMetrics();
    updateMarqueeState();
}

void MarqueeLabel::scrollText() {
    scrollOffset += 1;
    
    if (scrollOffset > textWidth + 172) {
        scrollOffset = 0;
        timer->stop();
        QTimer::singleShot(3000, this, [this]{ timer->start(50); });
    }
    
    update();
}

void MarqueeLabel::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    
    painter.fillRect(rect(), palette().window());
    painter.setClipRect(rect());
    
    QRect textRect = rect();
    if (timer->isActive()) {
        textRect.setX(rect().x() - scrollOffset);
        painter.drawText(textRect, alignment(), text());
        
        QRect loopRect = textRect;
        loopRect.setX(textRect.x() + textWidth + 172);
        painter.drawText(loopRect, alignment(), text());
    } else painter.drawText(textRect, alignment(), text());
}