#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QHash>
#include <QList>
#include <QFont>
#include <QFile>
#include <QLabel>
#include <QEvent>
#include <QWidget>
#include <QString>
#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QScrollBar>
#include <QFileDialog>
#include <QPushButton>
#include <QMainWindow>
#include <qtermwidget.h>
#include <QCoreApplication>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void splitVertical();
    void splitHorizontal();
    void openExplorer();
    void openMediaFile(const QString &path);
    void onKeyboardClicked();
    int keyboardMaxHeight(int currentScreenWidth) const;

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

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    
    void initShiftKeys();

    bool isAlt = false;
    bool isCtrl = false;
    bool isCaps = false;
    bool isShift = false;
    int ctrlPressCount = 0;
    int shiftPressCount = 0;
    QFileDialog *mediaDialog;
    QString pendingAccent = "";
    QList<QPushButton*> shiftBtns;
    QHash<QPushButton*, QString> shiftedTextMap;
    QHash<QPushButton*, QString> unshiftedTextMap;
    QPropertyAnimation *m_keyboardHeightAnimation = nullptr;

    // ---- Screen ----
    const int SCREEN_MIN_WIDTH = 640;
    const int SCREEN_MAX_WIDTH = 1920;

    // ---- Keyboard ----
    const int FRAME_MIN_WIDTH = 620;
    const int FRAME_MIN_HEIGHT = 215;
    const int FRAME_MAX_WIDTH = 930;
    const int FRAME_MAX_HEIGHT = 323;
    const int ANIMATION_DURATION = 300;
};

#endif // MAINWINDOW_H
