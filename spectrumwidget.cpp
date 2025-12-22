#include "spectrumwidget.h"

SpectrumWidget::SpectrumWidget(QWidget *parent)
    : QWidget(parent) {
    setStyleSheet("background-color: transparent; border: none;");
    
    decayTimer = new QTimer(this);
    connect(decayTimer, &QTimer::timeout, this, &SpectrumWidget::handleDecay);
    decayTimer->start(50);
}

void SpectrumWidget::updateSpectra(const QVector<double>& bands) {
    if (bands.size() != BAR_COUNT) return;
    
    for (int i = 0; i < BAR_COUNT; ++i)
        barHeights[i] = qMax(barHeights[i] * 0.2 + bands[i] * 0.8, barHeights[i]);
    update();
}

void SpectrumWidget::handleDecay() {
    bool updated = false;
    for (int i = 0; i < BAR_COUNT; ++i) {
        if (barHeights[i] > 0.0) {
            barHeights[i] *= DECAY_RATE; 
            if (barHeights[i] < 0.01) barHeights[i] = 0.0;
            updated = true;
        }
    }
    if (updated) update();
}

void SpectrumWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (barHeights.isEmpty()) return;

    int w = width();
    int h = height();

    const int BAR_SPACE = 3;
    const int BAR_PADDING = 3;
    const int BAR_ROUNDING = 6;

    const int totalBarAreaWidth = w - (2 * BAR_PADDING);
    const int totalSpaceBetweenBars = (BAR_COUNT - 1) * BAR_SPACE;
    const int BAR_WIDTH = (totalBarAreaWidth - totalSpaceBetweenBars) / BAR_COUNT;

    QLinearGradient gradient(0, h, 0, 0); // Bottom to End
    gradient.setColorAt(0.0, QColor("#1f2e40"));
    gradient.setColorAt(0.7, QColor("#99a0b2"));
    gradient.setColorAt(1.0, QColor(255, 255, 255));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);

    for (int i = 0; i < BAR_COUNT; ++i) {
        // ---- Logarithmic Scaling (Visual Adjustment) ----
        double magnitude = barHeights[i];

        // Exponential curve to visually boost smaller values (exponent < 1.0)
        double scaledMagnitude = qPow(magnitude, 0.85); 
        
        // Effective bar height limit (bottom margin)
        const int maxBarHeight = h - BAR_PADDING;
        int barH = static_cast<int>(scaledMagnitude * maxBarHeight);
        
        // Minimum visibility for low values (px)
        if (barH > 0 && barH < 2) barH = 3;

        // Position (X & Y)
        int x = BAR_PADDING + i * (BAR_WIDTH + BAR_SPACE);
        int y = h - BAR_PADDING - barH;
        
        // Draw Rounded Bar
        QRect barRect(x, y, BAR_WIDTH, barH);
        painter.drawRoundedRect(barRect, BAR_ROUNDING, BAR_ROUNDING);
    }
}