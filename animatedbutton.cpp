#include "animatedbutton.h"

const QList<QString> AnimatedButton::RESIZEABLE_BUTTONS = {
    "btnExplorer",
    "btnKeyboard",
    "btnSplitH",
    "btnSplitV"
};

AnimatedButton::AnimatedButton(QWidget *parent)
    : QPushButton(parent) {
    // ---- Background Color ----
    m_hoverBgColor = QColor("#05080e");
    m_bgAnimation = new QPropertyAnimation(this, "hoverBgColor", this);
    m_bgAnimation->setDuration(150);
    
    // ---- Border Color ----
    m_shiftBorderColor = QColor("#05080e");
    m_borderAnimation = new QPropertyAnimation(this, "shiftBorderColor", this);
    m_borderAnimation->setDuration(150);

    // ---- Caps Lock ----
    m_capsBgColor = QColor("#05080e");
    m_capsAnimation = new QPropertyAnimation(this, "capsBgColor", this);
    m_capsAnimation->setDuration(150);

    // ---- Icon size ----
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ---- Icon Fade ----
    if (!graphicsEffect()) {
        QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(opacityEffect);
        opacityEffect->setOpacity(1.0); 
    }

    this->installEventFilter(this);
    updateStyleSheet();
}

void AnimatedButton::updateStyleSheet() {
    QColor finalBgColor;

    if (m_capsBgColor == QColor("#05080e"))
        finalBgColor = m_hoverBgColor;
    else
        finalBgColor = QColor("#1f2e40");

    QString borderColor = m_shiftBorderColor.name();
    QString borderWidth = "0px";

    if (m_shiftBorderColor != QColor("#05080e"))
        borderWidth = "2px";

    QString style = QString(R"(
        QPushButton {
            font-weight: 700;
            border-radius: 6px;
            text-align: center;
            border: %1 solid %2;
            background-color: %3;
        }
        QPushButton:pressed {
            background-color: #1f2e40;
        }
            
        #btnSpace {
            border: 2px solid #99a0b2;
        }
    )").arg(borderWidth)
        .arg(borderColor)
        .arg(finalBgColor.name());

    this->setStyleSheet(style);
}

// ---- Background Color ----
void AnimatedButton::setHoverBgColor(const QColor &color) {
    if (m_hoverBgColor != color) {
        m_hoverBgColor = color;
        updateStyleSheet();
    }
}

bool AnimatedButton::eventFilter(QObject *obj, QEvent *event) {
    // Overrides button's hover event
    if (obj == this) {
        if (event->type() == QEvent::Enter) {
            m_bgAnimation->stop();
            m_bgAnimation->setStartValue(m_hoverBgColor);
            m_bgAnimation->setEndValue(QColor("#1f2e40")); 
            m_bgAnimation->start();
            return true;
        } else if (event->type() == QEvent::Leave) {
            m_bgAnimation->stop();
            m_bgAnimation->setStartValue(m_hoverBgColor);
            m_bgAnimation->setEndValue(QColor("#05080e"));
            m_bgAnimation->start();
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

// ---- Border Color ----
void AnimatedButton::setShiftBorderColor(const QColor &color) {
    if (m_shiftBorderColor != color) {
        m_shiftBorderColor = color;
        updateStyleSheet();
    }
}

void AnimatedButton::animateBorder(bool pressState) {
    m_borderAnimation->stop();
    m_borderAnimation->setStartValue(m_shiftBorderColor);
    
    if (pressState)
        m_borderAnimation->setEndValue(QColor("#99a0b2"));
    else
        m_borderAnimation->setEndValue(QColor("#05080e"));

    m_borderAnimation->start();
}

// ---- Caps Lock ----
void AnimatedButton::setCapsBgColor(const QColor &color) {
    if (m_capsBgColor != color) {
        m_capsBgColor = color;
        updateStyleSheet();
    }
}

void AnimatedButton::animateCapsState(bool isOn) {
    m_capsAnimation->stop();
    m_capsAnimation->setStartValue("#1f2e40");
    
    if (isOn) m_capsAnimation->setEndValue(QColor("#1f2e40"));
    else m_capsAnimation->setEndValue(QColor("#05080e"));

    m_capsAnimation->start();
}

// ---- Icon size ----
void AnimatedButton::resizeEvent(QResizeEvent *event) {
    QPushButton::resizeEvent(event);

    if (RESIZEABLE_BUTTONS.contains(this->objectName())) {
        int btnWidth = width();
        int btnHeight = height();
        int btnReference = (btnWidth <= btnHeight) ? btnWidth : btnHeight;
        int newIconSize = static_cast<int>(btnReference * 0.6);
        setIconSize(QSize(newIconSize, newIconSize));
    }
}

// ---- Icon Fade ----
void AnimatedButton::fadeIconSwap(const QIcon &newIcon) {
    QGraphicsOpacityEffect *opacityEffect = qobject_cast<QGraphicsOpacityEffect*>(graphicsEffect());

    if (!opacityEffect) {
        this->setIcon(newIcon);
        return;
    }
    
    QPropertyAnimation *fade = new QPropertyAnimation(opacityEffect, "opacity", this);
    fade->setDuration(150);
    fade->setEasingCurve(QEasingCurve::InQuad);
    
    QObject::connect(fade, &QPropertyAnimation::finished, this, [this, newIcon, fade](){
        this->setIcon(newIcon); 

        fade->disconnect();
        fade->setStartValue(0.0);
        fade->setEndValue(1.0);
        
        QObject::connect(fade, &QPropertyAnimation::finished, fade, &QObject::deleteLater, Qt::SingleShotConnection);
        
        fade->start();
    }, Qt::SingleShotConnection);
    
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    fade->start();
}