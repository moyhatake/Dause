#ifndef ANIMATEDBUTTON_H
#define ANIMATEDBUTTON_H

#include <QSize>
#include <QList>
#include <QColor>
#include <QEvent>
#include <QWidget>
#include <QObject>
#include <QString>
#include <QPushButton>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class AnimatedButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor hoverBgColor READ hoverBgColor WRITE setHoverBgColor)
    Q_PROPERTY(QColor shiftBorderColor READ shiftBorderColor WRITE setShiftBorderColor)
    Q_PROPERTY(QColor capsBgColor READ capsBgColor WRITE setCapsBgColor)

public:
    explicit AnimatedButton(QWidget *parent = nullptr);

    // ---- Background Color ----
    QColor hoverBgColor() const { return m_hoverBgColor; }
    void setHoverBgColor(const QColor &color);

    // ---- Border Color ----
    QColor shiftBorderColor() const { return m_shiftBorderColor; }
    void setShiftBorderColor(const QColor &color);
    void animateBorder(bool pressState);

    // ---- Caps Lock ----
    QColor capsBgColor() const { return m_capsBgColor; }
    void setCapsBgColor(const QColor &color);
    void animateCapsState(bool isOn);

    // ---- Icon Fade ----
    void fadeIconSwap(const QIcon &newIcon);

protected:
    // ---- Background Color ----
    bool eventFilter(QObject *obj, QEvent *event) override;

    // ---- Icon Size ----
    void resizeEvent(QResizeEvent *event) override;

private:
    // ---- Background Color ----
    QColor m_hoverBgColor;
    QPropertyAnimation *m_bgAnimation;

    // ---- Border Color ----
    QColor m_shiftBorderColor;
    QPropertyAnimation *m_borderAnimation;

    // ---- Caps Lock ----
    QColor m_capsBgColor;
    QPropertyAnimation *m_capsAnimation = nullptr;

    // ---- Icon Size ----
    static const QList<QString> RESIZEABLE_BUTTONS;

    void updateStyleSheet();
};

#endif // ANIMATEDBUTTON_H