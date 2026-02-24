#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QHash>
#include <QList>
#include <QFont>
#include <QFile>
#include <QStyle>
#include <QLabel>
#include <QEvent>
#include <QWidget>
#include <QString>
#include <QObject>
#include <QVector>
#include <QProcess>
#include <QSplitter>
#include <QSettings>
#include <QScrollBar>
#include <QMetaObject>
#include <QFileDialog>
#include <QPushButton>
#include <QMainWindow>
#include <qtermwidget.h>
#include <dauseterminal.h>
#include <QCoreApplication>
#include <QPropertyAnimation>
#include <QGraphicsColorizeEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void openMediaFile(const QString &path);

private slots:
    void splitVertical();
    void splitHorizontal();
    void openExplorer();
    void onKeyboardClicked();

    void onShiftPressed();
    void onShiftReleased();
    void onCtrlPressed();
    void onCtrlReleased();
    void onAltPressed();
    void onAltReleased();
    void onCapsClicked();
    void onKeyClicked();
    QString makeCSI(const QString& finalLetter, const QString& prefix);
    QString letterAndModifiers(QString letter);
    QString applyAccent(QString letter);
    QString getUserInfo();
    void keybindings();

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    
    void initShiftKeys();
    DauseTerminal* createTerminal();
    void replaceTerminalWithSplitter(Qt::Orientation orientation);
    int activeTerminals(QWidget *root);
    bool canSpawnTerm(Qt::Orientation orientation);
    void triggerAlert(QWidget *target);
    void closeTerminal(QTermWidget *target);

    bool isAlt = false;
    bool isCtrl = false;
    bool isCaps = false;
    bool isShift = false;
    int ctrlPressCount = 0;
    int shiftPressCount = 0;
    QFileDialog *mediaDialog;
    QString pendingAccent = "";
    QList<QPushButton*> shiftBtns;
    QSplitter *rootSplitter = nullptr;
    QTermWidget *activeTerminal = nullptr;
    QHash<QPushButton*, QString> shiftedTextMap;
    QHash<QPushButton*, QString> unshiftedTextMap;
    QPropertyAnimation *m_keyboardHeightAnimation = nullptr;

    // ---- Screen ----
    const int SCREEN_MIN_WIDTH = 640;
    const int SCREEN_MAX_WIDTH = 1920;

    // ---- Terminal ----
    const int HANDLE_WIDTH = 7;
    const int MAX_TERMINALS = 16;
    const int MIN_TERM_WIDTH = 142;
    const int MIN_TERM_HEIGHT = 80;

    // ---- Keyboard ----
    const int FRAME_MIN_WIDTH = 620;
    const int FRAME_MIN_HEIGHT = 215;
    const int FRAME_MAX_WIDTH = 930;
    const int FRAME_MAX_HEIGHT = 323;
    int targetKeyboardHeight = FRAME_MIN_HEIGHT;

    // ---- Styles ----
    static constexpr const char *APP_STYLE = R"(
        QMainWindow {
            border-radius: 6px;
            border: 2px solid #99a0b2;
        }

        #dockFrame {
            border-radius: 6px;
            border: 2px solid #99a0b2;
        }
        
        QToolTip {
            opacity: 128;
            padding: 0px 3px;
            color: #99a0b2;
            border-radius: 6px;
            border: 1px solid #99a0b2;
            background-color: #05080e;
        }

        QSplitter::handle {
            background-color: transparent;
        }
        QSplitter::handle:vertical {
            height: 7px;
        }
        QSplitter::handle:horizontal {
            width: 7px;
        }

        QMenu {
            color: #99a0b2;
            border-radius: 6px;
            border: 1px solid #99a0b2;
            background-color: rgba(5, 8, 14, 128);
        }
        QMenu::indicator {
            width: 0px;
            height: 0px;
        }
        QMenu::item {
            left: -10px;
            height: 26px;
            padding: 0 6px;
            min-width: 78px;
            border-radius: 6px;
        }
        QMenu::item:selected {
            background-color: rgba(31, 46, 64, 128);
        }
        QMenu::item:checked {
            color: #05080e;
            background-color: rgba(153, 160, 178, 128);
        }

        QDialog {
            color: #99a0b2;
            border-radius: 6px;
            border: 2px solid #99a0b2;
            background-color: #05080e;
        }
    )";
    static constexpr const char *SCROLL_STYLE = R"(
        QScrollBar:vertical {
            width: 7px;
            border: none;
            border-radius: 3px;
            background: transparent;
        }
        QScrollBar::add-line:vertical, 
        QScrollBar::sub-line:vertical {
            height: 0px;
            border: none;
            background: none;
            subcontrol-position: top;
            subcontrol-origin: margin;
        }
        QScrollBar::up-arrow:vertical, 
        QScrollBar::down-arrow:vertical {
            width: 0px;
            height: 0px;
        }
        QScrollBar::handle:vertical {
            border: none;
            min-height: 20px;
            border-radius: 3px;
            background: #1f2e40;
        }
        QScrollBar::handle:vertical:hover {
            background: #99a0b2;
        }
    )";
    static constexpr const char *EXPLORER_STYLE = R"(
        QFileDialog {
            color: #99a0b2;
            border-radius: 6px;
            background-color: #05080e;
            border: 2px solid #99a0b2;
        }

        QHeaderView::section {
            padding: 3px 0;
            border: none;
            height: 20px;
            color: #05080e;
            background-color: #99a0b2;
        }

        QPushButton, QToolButton {
            height: 26px;
            padding: 0 6px;
            color: #99a0b2;
            border-radius: 6px;
            background-color: transparent;
        }
        QPushButton:hover,
        QToolButton:hover {
            background-color: #1f2e40;
        }

        QListView, QTreeView {
            color: #99a0b2;
            background-color: #05080e;
            show-decoration-selected: 1;
        }
        QListView::item,
        QTreeView::item {
            height: 26px;
        }
        QListView::item:selected,
        QTreeView::item:selected {
            background-color: #1f2e40;
        }

        QComboBox {
            border: none;
            height: 26px;
            padding: 0 6px;
            color: #99a0b2;
            border-radius: 6px;
            background-color: #1f2e40;
        }
        QComboBox::drop-down {
            width: 26px;
            border-radius: 6px;
            background-color: #1f2e40;
        }
        QComboBox::down-arrow {
            image: url(:/icons/assets/drop_down.svg);
            width: 20px;
            height: 20px;
        }
        QComboBox QAbstractItemView {
            border: none;
            padding: 1px;
            height: 26px;
            color: #99a0b2;
            border-radius: 6px;
            background-color: #05080e;
            border: 1px solid #99a0b2;
            selection-background-color: #1f2e40;
        }

        QLineEdit {
            height: 22px;
            padding: 0 6px;
            color: #99a0b2;
            border-radius: 6px;
            border: 2px solid #99a0b2;
            background-color: transparent;
        }
        QLineEdit:focus {
            background-color: #1f2e40;
        }

        QScrollBar:vertical {
            width: 7px;
            border: none;
            border-radius: 3px;
            background: transparent;
        }
        QScrollBar::add-line:vertical, 
        QScrollBar::sub-line:vertical {
            height: 0px;
            border: none;
            background: none;
            subcontrol-position: top;
            subcontrol-origin: margin;
        }
        QScrollBar::up-arrow:vertical, 
        QScrollBar::down-arrow:vertical {
            width: 0px;
            height: 0px;
        }
        QScrollBar::handle:vertical {
            border: none;
            min-height: 20px;
            border-radius: 3px;
            background: #1f2e40;
        }
        QScrollBar::handle:vertical:hover {
            background: #99a0b2;
        }

        QScrollBar:horizontal {
            height: 7px;
            border: none;
            border-radius: 3px;
            background: transparent;
        }
        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal {
            width: 0px;
            border: none;
            background: none;
        }
        QScrollBar::left-arrow:horizontal,
        QScrollBar::right-arrow:horizontal {
            width: 0px;
            height: 0px;
        }
        QScrollBar::handle:horizontal {
            border: none;
            min-width: 20px;
            border-radius: 3px;
            background: #1f2e40;
        }
        QScrollBar::handle:horizontal:hover {
            background: #99a0b2;
        }
    )";
};

#endif // MAINWINDOW_H
