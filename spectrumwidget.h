#ifndef SPECTRUMWIDGET_H
#define SPECTRUMWIDGET_H

#include <QTimer>
#include <QtMath>
#include <QWidget>
#include <QVector>
#include <QPainter>

class SpectrumWidget : public QWidget {
    Q_OBJECT

public:
    explicit SpectrumWidget(QWidget *parent = nullptr);

public slots:
    void updateSpectra(const QVector<double>& bands);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    const int BAR_COUNT = 12;
    const double DECAY_RATE = 0.97;

    QTimer *decayTimer = nullptr;
    QVector<double> barHeights = QVector<double>(BAR_COUNT, 0.0);
    
private slots:
    void handleDecay();
};

#endif // SPECTRUMWIDGET_H