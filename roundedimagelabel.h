#ifndef ROUNDEDIMAGELABEL_H
#define ROUNDEDIMAGELABEL_H

#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>

class RoundedImageLabel : public QLabel {
    Q_OBJECT
    
public:
    explicit RoundedImageLabel(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // ROUNDEDIMAGELABEL_H