#include "animatedbutton.h"
#include "mainwindow.h"
#include "mediawindow.h"
#include "ui_mainwindow.h"

//#include <QDebug>

// ---- App ----
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("Dause Terminal ~ " + getUserInfo());

    // ---- Styling ----
    QString styleSheet = R"(
        QMainWindow {
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

        #dockFrame {
            border-radius: 6px;
            border: 2px solid #99a0b2;
        }
    )";
    QString scrollStyle = R"(
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
    QString explorerStyle = R"(
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
    this->setStyleSheet(styleSheet);
    
    // ---- Keyboard Init ----
    initShiftKeys();
    shiftBtns << ui->btnComma
              << ui->btnDot
              << ui->btnHyphen
              << ui->btnPlus
              << ui->btnVerBar
              << ui->btnExclam
              << ui->btnInter
              << ui->btnQuotes
              << ui->btnHash
              << ui->btnDollar
              << ui->btnAmpersand
              << ui->btnParenthesis
              << ui->btnBrackets
              << ui->btnCaret
              << ui->btnSlash
              << ui->btnAcute;
    ui->keyboardFrame->setVisible(false);
    m_keyboardHeightAnimation = new QPropertyAnimation(ui->keyboardFrame, "maximumHeight", this);
    m_keyboardHeightAnimation->setDuration(ANIMATION_DURATION);
    m_keyboardHeightAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    
    // ---- Terminal Init ----
    //ui->terminal->addCustomColorSchemeDir(QCoreApplication::applicationDirPath() + "/colors");
    ui->terminal->addCustomColorSchemeDir(":/schemes/colors");
    ui->terminal->setColorScheme("Dause");
    ui->terminal->setKeyboardCursorShape(QTermWidget::KeyboardCursorShape::IBeamCursor);
    ui->terminal->setScrollBarPosition(QTermWidget::ScrollBarRight);
    ui->terminal->setTerminalFont(QFont("Adwaita Mono", 11));
    ui->terminal->setTerminalSizeHint(false);
    ui->terminal->setBlinkingCursor(true);

    // ---- Working Directory ----
    ui->terminal->setShellProgram("/bin/zsh");
    ui->terminal->setWorkingDirectory(QDir::homePath());
    ui->terminal->startShellProgram();
    ui->terminal->changeDir(QDir::homePath());

    // ---- Scrollbar Style ----
    QScrollBar *scrollbar = ui->terminal->findChild<QScrollBar*>();
    if (scrollbar) scrollbar->setStyleSheet(scrollStyle);

    // ---- Startup: Terminal Clear & Scrollbar Config ----
    connect(ui->terminal, &QTermWidget::receivedData, this, [=](const QString &text) {
        static bool first = true;
        if (first) {
            first = false;
            ui->terminal->sendText("clear\n");
        }

        QScrollBar *scrollbar = ui->terminal->findChild<QScrollBar*>();
        if (scrollbar) {
            if (scrollbar->maximum() > 0) scrollbar->show();
            else scrollbar->hide();
        }
    });

    // ---- Media Explorer Init ----
    mediaDialog = new QFileDialog(this, "Open File");
    mediaDialog->setOption(QFileDialog::DontUseNativeDialog, true);
    mediaDialog->setFileMode(QFileDialog::ExistingFile);
    mediaDialog->setDirectory(QDir::homePath());
    mediaDialog->setStyleSheet(explorerStyle);
    mediaDialog->setWindowOpacity(0.75);
    mediaDialog->setNameFilter(
        "Images (*.png *.jpg *.jpeg *.webp *.gif);;"
        "Audio (*.mp3 *.m4a *.flac *.wav *.ogg);;"
        "Video (*.mp4 *.mkv *.avi *.mov *.flv)"
    );

    // ---- Close App ----
    connect(ui->terminal, &QTermWidget::finished, this, &MainWindow::close);

    // ---- Dock Buttons ----
    connect(ui->btnSplitV, &QPushButton::clicked, this, &MainWindow::splitVertical);
    connect(ui->btnSplitH, &QPushButton::clicked, this, &MainWindow::splitHorizontal);
    connect(ui->btnExplorer, &QPushButton::clicked, this, &MainWindow::openExplorer);
    connect(mediaDialog, &QFileDialog::fileSelected, this, &MainWindow::openMediaFile);
    connect(ui->btnKeyboard, &QPushButton::clicked, this, &MainWindow::onKeyboardClicked);
    
    // ---- Keyboard Keys ----
    connect(ui->btnShiftL, &QPushButton::pressed, this, &MainWindow::onShiftPressed);
    connect(ui->btnShiftL, &QPushButton::released, this, &MainWindow::onShiftReleased);
    connect(ui->btnShiftR, &QPushButton::pressed, this, &MainWindow::onShiftPressed);
    connect(ui->btnShiftR, &QPushButton::released, this, &MainWindow::onShiftReleased);
    connect(ui->btnCtrl, &QPushButton::pressed, this, &MainWindow::onCtrlPressed);
    connect(ui->btnCtrl, &QPushButton::released, this, &MainWindow::onCtrlReleased);
    connect(ui->btnAlt, &QPushButton::pressed, this, &MainWindow::onAltPressed);
    connect(ui->btnAlt, &QPushButton::released, this, &MainWindow::onAltReleased);
    connect(ui->btnCaps, &QPushButton::clicked, this, &MainWindow::onCapsClicked);

    connect(ui->btnOne, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnTwo, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnThree, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnFour, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnFive, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnSix, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnSeven, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnEight, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnNine, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnZero, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnSpace, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnTab, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnEnter, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    
    connect(ui->btnEsc, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnPgUp, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnPgDown, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnHome, &QPushButton::clicked, this, &MainWindow::onKeyClicked);
    connect(ui->btnEnd, &QPushButton::clicked, this, &MainWindow::onKeyClicked);
    connect(ui->btnUp, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnDown, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnRight, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnLeft, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnBack, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnDel, &QPushButton::pressed, this, &MainWindow::onKeyClicked);

    connect(ui->btnQuotes, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnParenthesis, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnBrackets, &QPushButton::pressed, this, &MainWindow::onKeyClicked);

    connect(ui->btnVerBar, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnExclam, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnInter, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnAcute, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnHash, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnDollar, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnAmpersand, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnCaret, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnSlash, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnPlus, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnHyphen, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnComma, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnDot, &QPushButton::pressed, this, &MainWindow::onKeyClicked);

    connect(ui->btnQ, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnW, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnE, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnR, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnT, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnY, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnU, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnI, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnO, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnP, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnA, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnS, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnD, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnF, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnG, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnH, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnJ, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnK, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnL, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnZ, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnX, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnC, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnV, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnB, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnN, &QPushButton::pressed, this, &MainWindow::onKeyClicked);
    connect(ui->btnM, &QPushButton::pressed, this, &MainWindow::onKeyClicked);

    // ---- GUI Shortcuts ----
    qApp->installEventFilter(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

// ---- UI ----
void MainWindow::splitVertical() {

}

void MainWindow::splitHorizontal() {

}

void MainWindow::initShiftKeys() {
    // ---- Unshifted Text ----
    unshiftedTextMap[ui->btnVerBar]      = "|";
    unshiftedTextMap[ui->btnExclam]      = "!";
    unshiftedTextMap[ui->btnInter]       = "?";
    unshiftedTextMap[ui->btnQuotes]      = "\"";
    unshiftedTextMap[ui->btnAcute]       = "´";
    unshiftedTextMap[ui->btnHash]        = "#";
    unshiftedTextMap[ui->btnDollar]      = "$";
    unshiftedTextMap[ui->btnAmpersand]   = "&&";
    unshiftedTextMap[ui->btnParenthesis] = "(";
    unshiftedTextMap[ui->btnBrackets]    = "{";
    unshiftedTextMap[ui->btnCaret]       = "^";
    unshiftedTextMap[ui->btnSlash]       = "/";
    unshiftedTextMap[ui->btnPlus]        = "+";
    unshiftedTextMap[ui->btnHyphen]      = "-";
    unshiftedTextMap[ui->btnComma]       = ",";
    unshiftedTextMap[ui->btnDot]         = ".";

    // ---- Shifted Text ----
    shiftedTextMap[ui->btnVerBar]      = "°";
    shiftedTextMap[ui->btnExclam]      = "¡";
    shiftedTextMap[ui->btnInter]       = "¿";
    shiftedTextMap[ui->btnQuotes]      = "'";
    shiftedTextMap[ui->btnAcute]       = "`";
    shiftedTextMap[ui->btnHash]        = "@";
    shiftedTextMap[ui->btnDollar]      = "%";
    shiftedTextMap[ui->btnAmpersand]   = "=";
    shiftedTextMap[ui->btnParenthesis] = "<";
    shiftedTextMap[ui->btnBrackets]    = "[";
    shiftedTextMap[ui->btnCaret]       = "~";
    shiftedTextMap[ui->btnSlash]       = "\\";
    shiftedTextMap[ui->btnPlus]        = "*";
    shiftedTextMap[ui->btnHyphen]      = "_";
    shiftedTextMap[ui->btnComma]       = ";";
    shiftedTextMap[ui->btnDot]         = ":";
}

void MainWindow::openExplorer() {
    mediaDialog->open();
}

void MainWindow::openMediaFile(const QString &path) {
    MediaWindow *viewer = new MediaWindow(path);
    viewer->setAttribute(Qt::WA_DeleteOnClose);
    viewer->show();
}

void MainWindow::onKeyboardClicked() {
    AnimatedButton *dockKeyboard = qobject_cast<AnimatedButton*>(ui->btnKeyboard);
    QIcon keyboardIcon;

    if (m_keyboardHeightAnimation->state() == QAbstractAnimation::Running)
        m_keyboardHeightAnimation->stop();

    int targetHeight = keyboardMaxHeight(width());

    if (ui->keyboardFrame->isVisible() && ui->keyboardFrame->maximumHeight() >= FRAME_MIN_HEIGHT) {
        // Hide
        keyboardIcon = QIcon(":/icons/assets/keyboard_s.svg");

        ui->keyboardFrame->setMinimumHeight(0);

        m_keyboardHeightAnimation->setStartValue(ui->keyboardFrame->height());
        m_keyboardHeightAnimation->setEndValue(0);
        m_keyboardHeightAnimation->start();

        QObject::connect(m_keyboardHeightAnimation, &QPropertyAnimation::finished, this, [this, targetHeight]() {
            ui->keyboardFrame->setVisible(false);
            ui->keyboardFrame->setMinimumHeight(FRAME_MIN_HEIGHT);
            ui->keyboardFrame->setMaximumHeight(targetHeight);
            ui->btnKeyboard->setToolTip("Show Keyboard");
        }, Qt::SingleShotConnection);
    } else {
        // Show
        keyboardIcon = QIcon(":/icons/assets/keyboard_h.svg");

        ui->keyboardFrame->setMaximumHeight(0);
        ui->keyboardFrame->setVisible(true);

        m_keyboardHeightAnimation->setStartValue(0);
        m_keyboardHeightAnimation->setEndValue(targetHeight);
        m_keyboardHeightAnimation->start();

        QObject::connect(m_keyboardHeightAnimation, &QPropertyAnimation::finished, this, [this, targetHeight]() {
            ui->keyboardFrame->setMinimumHeight(FRAME_MIN_HEIGHT);
            ui->keyboardFrame->setMaximumHeight(targetHeight);
            ui->btnKeyboard->setToolTip("Hide Keyboard");
        }, Qt::SingleShotConnection);
    }

    if (dockKeyboard) {
        dockKeyboard->fadeIconSwap(keyboardIcon);
        dockKeyboard->animateCapsState(false);
    }
    
    ui->terminal->setFocus();
}

int MainWindow::keyboardMaxHeight(int currentScreenWidth) const {
    if (currentScreenWidth <= SCREEN_MIN_WIDTH)
        return FRAME_MAX_HEIGHT;

    const int SCREEN_WIDTH_RANGE = SCREEN_MAX_WIDTH - SCREEN_MIN_WIDTH;
    const int FRAME_WIDTH_RANGE = FRAME_MAX_WIDTH - FRAME_MIN_WIDTH;
    const int FRAME_HEIGHT_RANGE = FRAME_MAX_HEIGHT - FRAME_MIN_HEIGHT;

    int widthDifference = currentScreenWidth - SCREEN_MIN_WIDTH;

    double scaleFactor = 0.0;
    if (currentScreenWidth < SCREEN_MAX_WIDTH)
        scaleFactor = (double)widthDifference / SCREEN_WIDTH_RANGE;
    else
        scaleFactor = 1.0;
        
    int newMaxHeight = FRAME_MIN_HEIGHT + qRound(FRAME_HEIGHT_RANGE * scaleFactor);

    return qMin(newMaxHeight, FRAME_MAX_HEIGHT);
}

void MainWindow::onShiftPressed() {
    shiftPressCount++;

    if (shiftPressCount == 1) {
        isShift = true;
        
        for (QPushButton *button : shiftBtns) {
            AnimatedButton *animButton = qobject_cast<AnimatedButton*>(button);

            if (animButton) animButton->animateBorder(true);
            if (shiftedTextMap.contains(button)) button->setText(shiftedTextMap[button]);
        }
    }
}

void MainWindow::onShiftReleased() {
    if (shiftPressCount > 0) shiftPressCount--;

    if (shiftPressCount == 0) {
        isShift = false;

        for (QPushButton *button : shiftBtns) {
            AnimatedButton *animButton = qobject_cast<AnimatedButton*>(button);
            
            if (animButton) animButton->animateBorder(false);
            if (unshiftedTextMap.contains(button)) button->setText(unshiftedTextMap[button]);
        }
        
        ui->terminal->setFocus();
    }
}

void MainWindow::onCtrlPressed() {
    ctrlPressCount++;

    if (ctrlPressCount == 1) isCtrl = true;
}

void MainWindow::onCtrlReleased() {
    if (ctrlPressCount > 0) ctrlPressCount--;

    if (ctrlPressCount == 0) isCtrl = false;
}

void MainWindow::onAltPressed() {
    isAlt = true;
}

void MainWindow::onAltReleased() {
    isAlt = false;
}

void MainWindow::onCapsClicked() {
    isCaps = !isCaps;

    AnimatedButton *capsButton = qobject_cast<AnimatedButton*>(ui->btnCaps);
    if (capsButton) capsButton->animateCapsState(isCaps);

    ui->terminal->setFocus();    
}

void MainWindow::onKeyClicked() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString keyValue = "";

    // Numbers
    if (button == ui->btnOne) keyValue = "1";
    else if (button == ui->btnTwo) keyValue = "2";
    else if (button == ui->btnThree) keyValue = "3";
    else if (button == ui->btnFour) keyValue = "4";
    else if (button == ui->btnFive) keyValue = "5";
    else if (button == ui->btnSix) keyValue = "6";
    else if (button == ui->btnSeven) keyValue = "7";
    else if (button == ui->btnEight) keyValue = "8";
    else if (button == ui->btnNine) keyValue = "9";
    else if (button == ui->btnZero) keyValue = "0";

    // Static Keys
    else if (button == ui->btnSpace) keyValue = " ";
    else if (button == ui->btnTab) keyValue = "\t";
    else if (button == ui->btnEnter) keyValue = "\r";
    else if (button == ui->btnEsc) keyValue = "\x1B";
    else if (button == ui->btnBack) keyValue = "\x7F";

    // Arrows
    else if (button == ui->btnUp) keyValue = makeCSI("A", "1");
    else if (button == ui->btnDown) keyValue = makeCSI("B", "1");
    else if (button == ui->btnRight) keyValue = makeCSI("C", "1");
    else if (button == ui->btnLeft) keyValue = makeCSI("D", "1");
    
    // Home / End
    else if (button == ui->btnHome) keyValue = makeCSI("H", "1");
    else if (button == ui->btnEnd) keyValue = makeCSI("F", "1");
    
    // Delete / Page Up / Page Down
    else if (button == ui->btnDel) keyValue = makeCSI("~", "3");
    else if (button == ui->btnPgUp) keyValue = makeCSI("~", "5");
    else if (button == ui->btnPgDown)keyValue = makeCSI("~", "6");
    
    // Paired Symbols
    else if (button == ui->btnQuotes) {
        if (isShift) keyValue = "''\x1B[D";
        else keyValue = "\"\"\x1B[D";
    } else if (button == ui->btnParenthesis) {
        if (isShift) keyValue = "<>\x1B[D";
        else keyValue = "()\x1B[D";
    } else if (button == ui->btnBrackets) {
        if (isShift) keyValue = "[]\x1B[D";
        else keyValue = "{}\x1B[D";
    }
    
    // Single Symbols
    else if (unshiftedTextMap.contains(button)
        && button != ui->btnQuotes
        && button != ui->btnParenthesis
        && button != ui->btnBrackets) {
            if (isShift) {
                if (button == ui->btnAcute) pendingAccent = "`";
                else if (button == ui->btnCaret) pendingAccent = "~";
                else keyValue = shiftedTextMap.value(button);
            } else {
                // Since & key is set '&&' in Qt Creator
                if (button == ui->btnAmpersand) keyValue = "&";
                else if (button == ui->btnAcute) pendingAccent = "´";
                else if (button == ui->btnCaret) pendingAccent = "^";
                else keyValue = unshiftedTextMap.value(button);
            }
    }

    // Letters
    else if (button->objectName().length() == 4) {
        QString letter = button->objectName().mid(3).toLower();
        keyValue = letterAndModifiers(letter);
    }

    ui->terminal->sendText(keyValue);
    ui->terminal->setFocus();

    // HOME & END keys get stuck if setDown(true)
    if (button != ui->btnHome && button != ui->btnEnd) button->setDown(true);
}

QString MainWindow::makeCSI(const QString& finalLetter, const QString& prefix) {
    int mod = 1;

    if (isShift) mod += 1;
    if (isAlt) mod += 2;
    if (isCtrl) mod += 4;

    if (mod == 1) return QString("\x1B[%1%2").arg(prefix).arg(finalLetter);

    return QString("\x1B[%1;%2%3").arg(prefix).arg(mod).arg(finalLetter);
}

QString MainWindow::letterAndModifiers(QString letter) {
    // GUI Shortcuts
    if (isCtrl && isShift) {
        if (letter == "c") {
            ui->terminal->copyClipboard();
            return QString();
        }
        if (letter == "v") {
            ui->terminal->pasteClipboard();
            return QString();
        }
        /*if (letter == "a") { // Double-check
            int rows = ui->terminal->historyLinesCount();
            int columns = ui->terminal->screenColumnsCount();
            ui->terminal->setSelectionStart(0, 0);
            ui->terminal->setSelectionEnd(rows - 1, columns - 1);
            return QString();
        }*/
        if (letter == "q" || letter == "w") {
            this->close();
            return QString();
        }
    }
    
    // Base Modifiers
    if (isCtrl) {
        if (letter == "d") return QString("\x04"); // EOT

        QChar c = letter[0].toLower();
        char ctrl = c.unicode() - 96;
        return QString(ctrl);
    }

    if (isAlt) {
        if (isShift || isCaps) return "\x1B" + letter.toUpper();
        else return "\x1B" + letter.toLower();
    }

    if (isShift || isCaps) return applyAccent(letter.toUpper());
    return applyAccent(letter.toLower());
}

QString MainWindow::applyAccent(QString letter) {
    if (pendingAccent.isEmpty()) return letter;

    QString result = letter;    

    // Acute (´)
    static QMap<QString, QString> acute = {
        {"a","á"}, {"e","é"}, {"i","í"}, {"o","ó"}, {"u","ú"}, {"y","ý"},
        {"A","Á"}, {"E","É"}, {"I","Í"}, {"O","Ó"}, {"U","Ú"}, {"Y","Ý"},
        {"c","ć"}, {"g","ǵ"}, {"k","ḱ"}, {"l","ĺ"}, {"m","ḿ"}, {"n","ń"},
        {"C","Ć"}, {"G","Ǵ"}, {"K","Ḱ"}, {"L","Ĺ"}, {"M","Ḿ"}, {"N","Ń"},
        {"p","ṕ"}, {"r","ŕ"}, {"s","ś"}, {"v","ǘ"}, {"w","ẃ"}, {"z","ź"},
        {"P","Ṕ"}, {"R","Ŕ"}, {"S","Ś"}, {"V","Ǘ"}, {"W","Ẃ"}, {"Z","Ź"}
    };

    // Grave (`)
    static QMap<QString, QString> grave = {
        {"a","à"}, {"e","è"}, {"i","ì"}, {"o","ò"}, {"u","ù"},
        {"A","À"}, {"E","È"}, {"I","Ì"}, {"O","Ò"}, {"U","Ù"},
        {"n","ǹ"}, {"v","ǜ"}, {"w","ẁ"}, {"y","ỳ"},
        {"N","Ǹ"}, {"V","Ǜ"}, {"W","Ẁ"}, {"Y","Ỳ"}
    };

    // Caret (^)
    static QMap<QString, QString> caret = {
        {"a","â"}, {"e","ê"}, {"i","î"}, {"o","ô"}, {"u","û"}, {"y", "ŷ"},
        {"A","Â"}, {"E","Ê"}, {"I","Î"}, {"O","Ô"}, {"U","Û"}, {"Y","Ŷ"},
        {"c","ĉ"}, {"g","ĝ"}, {"h","ĥ"}, {"j","ĵ"}, {"s","ŝ"}, {"w","ŵ"}, {"z","ẑ"},
        {"C","Ĉ"}, {"G","Ĝ"}, {"H","Ĥ"}, {"J","Ĵ"}, {"S","Ŝ"}, {"W","Ŵ"}, {"Z","Ẑ"}
    };

    // Tilde (~)
    static QMap<QString, QString> tilde = {
        {"a","ã"}, {"e","ẽ"}, {"i","ĩ"}, {"o","õ"}, {"u","ũ"},
        {"A","Ã"}, {"E","Ẽ"}, {"I","Ĩ"}, {"O","Õ"}, {"U","Ũ"},
        {"n","ñ"}, {"v","ṽ"}, {"y","ỹ"},
        {"N","Ñ"}, {"V","Ṽ"}, {"Y","Ỹ"}
    };

    if (pendingAccent == "´" && acute.contains(letter))
        result = acute[letter];    
    else if (pendingAccent == "`" && grave.contains(letter))
        result = grave[letter];
    else if (pendingAccent == "^" && caret.contains(letter))
        result = caret[letter];
    else if (pendingAccent == "~" && tilde.contains(letter))
        result = tilde[letter];
    else result = pendingAccent + letter;

    pendingAccent.clear();
    return result;
}

QString MainWindow::getUserInfo() {
    QSettings settings(QCoreApplication::applicationDirPath() + "/host.ini", QSettings::IniFormat);

    settings.beginGroup("HostInfo");
    QString stored_username = settings.value("username").toString();
    QString stored_hostname = settings.value("hostname").toString();
    settings.endGroup();

    bool config_exists = !stored_username.isEmpty() && !stored_hostname.isEmpty();
    
    QString current_username;
    QString current_hostname;
    
    QProcess usernameProcess;
    usernameProcess.start("whoami"); 
    usernameProcess.waitForFinished();
    current_username = usernameProcess.readAllStandardOutput().trimmed();

    QProcess hostnameProcess;
    hostnameProcess.start("hostname");
    hostnameProcess.waitForFinished();
    current_hostname = hostnameProcess.readAllStandardOutput().trimmed();

    bool needs_save = false;
    QString final_username = current_username;
    QString final_hostname = current_hostname;

    if (config_exists) {
        if (stored_username != current_username || stored_hostname != current_hostname)
            needs_save = true;
    } else needs_save = true;

    if (needs_save) {
        settings.beginGroup("HostInfo");
        settings.setValue("username", final_username);
        settings.setValue("hostname", final_hostname);
        settings.endGroup();
    }

    return final_username + "@" + final_hostname;
}

// ---- Screen/Keyboard Resizing
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    int currentScreenWidth = event->size().width();

    if (currentScreenWidth <= SCREEN_MIN_WIDTH) {
        ui->keyboardFrame->setMinimumSize(FRAME_MIN_WIDTH, FRAME_MIN_HEIGHT);
        ui->keyboardFrame->setMaximumSize(FRAME_MAX_WIDTH, FRAME_MAX_HEIGHT); 
        return; 
    }

    const int SCREEN_WIDTH_RANGE = SCREEN_MAX_WIDTH - SCREEN_MIN_WIDTH;
    const int FRAME_WIDTH_RANGE = FRAME_MAX_WIDTH - FRAME_MIN_WIDTH;
    const int FRAME_HEIGHT_RANGE = FRAME_MAX_HEIGHT - FRAME_MIN_HEIGHT;

    int widthDifference = currentScreenWidth - SCREEN_MIN_WIDTH;

    double scaleFactor = 0.0;
    if (currentScreenWidth < SCREEN_MAX_WIDTH)
        scaleFactor = (double)widthDifference / SCREEN_WIDTH_RANGE;
    else scaleFactor = 1.0;
    
    int newMaxWidth = FRAME_MIN_WIDTH + qRound(FRAME_WIDTH_RANGE * scaleFactor);
    int newMaxHeight = FRAME_MIN_HEIGHT + qRound(FRAME_HEIGHT_RANGE * scaleFactor);

    newMaxWidth = qMin(newMaxWidth, FRAME_MAX_WIDTH);
    newMaxHeight = qMin(newMaxHeight, FRAME_MAX_HEIGHT);
    
    ui->keyboardFrame->setMinimumSize(FRAME_MIN_WIDTH, FRAME_MIN_HEIGHT);
    ui->keyboardFrame->setMaximumSize(newMaxWidth, newMaxHeight);
    ui->keyboardFrame->resize(newMaxWidth, newMaxHeight);
    ui->keyboardFrame->updateGeometry();
}

// ---- GUI Shortcuts ----
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Shift) onShiftPressed();
        if (keyEvent->key() == Qt::Key_Control) onCtrlPressed();

        if (isCtrl && isShift) {
            if (keyEvent->key() == Qt::Key_C) {
                ui->terminal->copyClipboard();
                return true;
            }
            if (keyEvent->key() == Qt::Key_V) {
                ui->terminal->pasteClipboard();
                return true;
            }
            /*if (keyEvent->key() == Qt::Key_A) { // Double-check
                int rows = ui->terminal->historyLinesCount();
                int columns = ui->terminal->screenColumnsCount();
                ui->terminal->setSelectionStart(0, 0);
                ui->terminal->setSelectionEnd(rows - 1, columns - 1);
                return true;
            }*/
            if (keyEvent->key() == Qt::Key_Q || keyEvent->key() == Qt::Key_W) {
                this->close();
                return true;
            }
        }
    }

    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        
        if (keyEvent->key() == Qt::Key_Shift) onShiftReleased();
        if (keyEvent->key() == Qt::Key_Control) onCtrlReleased();
    }

    return QMainWindow::eventFilter(obj, event);
}