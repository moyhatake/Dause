#include "dauseterminal.h"

DauseTerminal::DauseTerminal(QWidget *parent) : QWidget(parent) {
    setMinimumSize(MIN_TERM_WIDTH, MIN_TERM_HEIGHT);

    setAutoFillBackground(true);
    setAttribute(Qt::WA_OpaquePaintEvent, true); 
    setAttribute(Qt::WA_NoSystemBackground, false);
    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(TERMINAL_STYLE);

    m_terminal = new QTermWidget(0, this);
    m_terminal->setStyleSheet("QTermWidget { border: none; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->setSpacing(0);
    layout->addWidget(m_terminal);
    setLayout(layout);
}

void DauseTerminal::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}