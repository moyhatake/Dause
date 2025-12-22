#ifndef MARQUEELABEL_H
#define MARQUEELABEL_H

#include <QLabel>
#include <QTimer>
#include <QFontMetrics>

class MarqueeLabel : public QLabel {
    Q_OBJECT

public:
    explicit MarqueeLabel(QWidget *parent = nullptr);
    void setText(const QString &text);
    void setMarqueeEnabled(bool enabled);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void scrollText();

private:
    int textWidth = 0;
    int scrollOffset = 0;
    QTimer *timer = nullptr;
    bool marqueeEnabled = true;

    void updateMetrics();
    void updateMarqueeState();
};

#endif // MARQUEELABEL_H