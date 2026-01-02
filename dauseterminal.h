#ifndef DAUSETERMINAL_H
#define DAUSETERMINAL_H

#include <QWidget>
#include <QPainter>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QStyleOption>
#include <qtermwidget.h>

class DauseTerminal : public QWidget {
    Q_OBJECT

public:
    explicit DauseTerminal(QWidget *parent = nullptr);
    QTermWidget* term() const { return m_terminal; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QTermWidget *m_terminal;

    // ---- Terminal ----
    const int MIN_TERM_WIDTH = 142;
    const int MIN_TERM_HEIGHT = 80;

    // ---- Styles ----
    static constexpr const char *TERMINAL_STYLE = R"(
        DauseTerminal {
            border-radius: 6px;
            border: 2px solid #1f2e40;
            background-color: #05080e;
        }
        DauseTerminal[focused="true"] {
            border: 2px solid #99a0b2;
        }
    )";
};

#endif // DAUSETERMINAL_H