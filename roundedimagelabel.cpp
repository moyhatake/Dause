#include "roundedimagelabel.h"

RoundedImageLabel::RoundedImageLabel(QWidget *parent) 
    : QLabel(parent) {}

void RoundedImageLabel::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addRoundedRect(rect(), 6, 6);

    painter.setClipPath(path);

    QPixmap p = pixmap(Qt::ReturnByValue);    
    if (!p.isNull()) {
        painter.drawPixmap(
            rect(), 
            p.scaled(
                size(), 
                Qt::KeepAspectRatioByExpanding,
                Qt::SmoothTransformation
            )
        );
    }
}