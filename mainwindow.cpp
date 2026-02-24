#include "animatedbutton.h"
#include "mainwindow.h"
#include "mediawindow.h"
#include "ui_mainwindow.h"

// ---- App ----
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("Dause Terminal ~ " + getUserInfo());
    this->setStyleSheet(APP_STYLE);
    
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
    m_keyboardHeightAnimation->setDuration(300);
    m_keyboardHeightAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    
    // ---- Base Terminal ----
    rootSplitter = new QSplitter(Qt::Horizontal, ui->terminalWrapper);
    rootSplitter->setOpaqueResize(false);
    rootSplitter->setContentsMargins(0, 0, 0, 0);
    rootSplitter->setChildrenCollapsible(false);
    rootSplitter->setStretchFactor(0, 1);

    QHBoxLayout *layout = new QHBoxLayout(ui->terminalWrapper);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(rootSplitter);

    DauseTerminal *baseTerm = createTerminal();
    rootSplitter->addWidget(baseTerm);
    baseTerm->term()->sendText("clear\n");
    baseTerm->term()->setFocus();

    // ---- Media Explorer Init ----
    mediaDialog = new QFileDialog(this, "Open File");
    mediaDialog->setOption(QFileDialog::DontUseNativeDialog, true);
    mediaDialog->setFileMode(QFileDialog::ExistingFile);
    mediaDialog->setDirectory(QDir::homePath());
    mediaDialog->setStyleSheet(EXPLORER_STYLE);
    mediaDialog->setWindowOpacity(0.75);
    mediaDialog->setNameFilter(
        "Images (*.png *.jpg *.jpeg *.webp *.gif);;"
        "Audio (*.mp3 *.m4a *.flac *.wav *.ogg);;"
        "Video (*.mp4 *.mkv *.avi *.mov *.flv)"
    );

    // ---- Dock Buttons ----
    connect(ui->btnSplitV, &QPushButton::clicked, this, &MainWindow::splitVertical);
    connect(ui->btnSplitH, &QPushButton::clicked, this, &MainWindow::splitHorizontal);
    connect(ui->btnExplorer, &QPushButton::clicked, this, &MainWindow::openExplorer);
    connect(mediaDialog, &QFileDialog::fileSelected, this, &MainWindow::openMediaFile);
    connect(ui->btnKeyboard, &QPushButton::clicked, this, &MainWindow::onKeyboardClicked);
    connect(ui->btnKeybind, &QPushButton::clicked, this, &MainWindow::keybindings);
    
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

DauseTerminal* MainWindow::createTerminal() {
    DauseTerminal *wrapper = new DauseTerminal(nullptr);
    QTermWidget *term = wrapper->term();

    QFont terminalFont("Adwaita Mono");
    terminalFont.setStyleHint(QFont::Monospace);
    terminalFont.setFixedPitch(true);
    terminalFont.setPointSize(11);

    term->setTerminalFont(terminalFont);
    term->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    term->setBlinkingCursor(true);
    term->setTerminalSizeHint(false);
    term->setScrollBarPosition(QTermWidget::ScrollBarRight);
    term->setKeyboardCursorShape(QTermWidget::KeyboardCursorShape::IBeamCursor);
    term->addCustomColorSchemeDir(":/schemes/colors");
    term->setColorScheme("Dause");
    term->setShellProgram("/bin/zsh");
    term->setArgs(QStringList() << "-i");
    term->setWorkingDirectory(QDir::homePath());
    term->startShellProgram();

    // ---- Scrollbar Style ----
    if (auto sb = term->findChild<QScrollBar*>()) sb->setStyleSheet(SCROLL_STYLE);

    // ---- Scrollbar Config ----
    connect(term, &QTermWidget::receivedData, this, [=] {
        if (auto sb = term->findChild<QScrollBar*>()) {
            if (sb->maximum() > 0) sb->show();
            else sb->hide();
        }
    });

    // ---- Close Terminal/App ----
    connect(term, &QTermWidget::finished, this, [=] { closeTerminal(term); });

    term->installEventFilter(this);
    return wrapper;
}

void MainWindow::replaceTerminalWithSplitter(Qt::Orientation orientation) {
    if (!activeTerminal) return;

    if (!canSpawnTerm(orientation)) {
        triggerAlert(activeTerminal->parentWidget());
        return;
    }

    QWidget *activeWrapper = activeTerminal->parentWidget();
    QSplitter *parentSplitter = qobject_cast<QSplitter*>(activeWrapper->parentWidget());

    if (parentSplitter && parentSplitter->orientation() == orientation) {
        // ---- Same orientation (EXPANDING, NO NESTING) ----
        DauseTerminal *newWrapper = createTerminal();
        int index = parentSplitter->indexOf(activeWrapper);
        parentSplitter->insertWidget(index + 1, newWrapper);
        newWrapper->term()->setFocus();
        
        QList<int> sizes;
        for(int i = 0; i < parentSplitter->count(); ++i) sizes << 10000;
        parentSplitter->setSizes(sizes);
    } else {
        // ---- Different orientation ----
        // Parent's splitter children = 1: Orientation changes (NO NESTING)
        if (parentSplitter && parentSplitter->count() == 1) {
            parentSplitter->setOrientation(orientation);
            replaceTerminalWithSplitter(orientation); 
            return;
        }

        // Parent's splitter children > 1 : New splitter (NESTING)
        QSplitter *newSplitter = new QSplitter(orientation);
        newSplitter->setOpaqueResize(false);
        newSplitter->setContentsMargins(0, 0, 0, 0);
        newSplitter->setChildrenCollapsible(false);

        DauseTerminal *newWrapper = createTerminal();
        int index = parentSplitter ? parentSplitter->indexOf(activeWrapper) : -1;
        newSplitter->addWidget(activeWrapper);
        newSplitter->addWidget(newWrapper);
        
        if (parentSplitter) parentSplitter->insertWidget(index, newSplitter);
        else rootSplitter->addWidget(newSplitter);
        
        newWrapper->term()->setFocus();

        newSplitter->setSizes({10000, 10000});
        
        QSplitter *targetParent = parentSplitter ? parentSplitter : rootSplitter;
        QList<int> pSizes;
        for(int i=0; i < targetParent->count(); ++i) pSizes << 10000;
        targetParent->setSizes(pSizes);
    }
}

int MainWindow::activeTerminals(QWidget *root) {
    return root->findChildren<QTermWidget*>().count();
}

bool MainWindow::canSpawnTerm(Qt::Orientation orientation) {
    if (!activeTerminal || activeTerminals(rootSplitter) >= 16) return false;

    QWidget *activeWrapper = activeTerminal->parentWidget();
    QSplitter *parentSplitter = qobject_cast<QSplitter*>(activeWrapper->parentWidget());

    // Same orientation (EXPANDING, NO NESTING)
    if (parentSplitter && parentSplitter->orientation() == orientation) {
        int nextCount = parentSplitter->count() + 1;
        if (orientation == Qt::Vertical) {
            int totalRequired = FRAME_MIN_HEIGHT + (nextCount * MIN_TERM_HEIGHT) + ((nextCount - 1) * HANDLE_WIDTH);
            return parentSplitter->height() >= totalRequired;
        } else {
            int totalRequired = (nextCount * MIN_TERM_WIDTH) + ((nextCount - 1) * HANDLE_WIDTH);
            return parentSplitter->width() >= totalRequired;
        }
    }

    // Different orientation (NESTING)
    if (orientation == Qt::Vertical) {
        int minRequired = FRAME_MIN_HEIGHT + (MIN_TERM_HEIGHT * 2) + HANDLE_WIDTH;
        return activeWrapper->height() >= minRequired;
    } else {
        int minRequired = (MIN_TERM_WIDTH * 2) + HANDLE_WIDTH;
        return activeWrapper->width() >= minRequired;
    }
}

void MainWindow::triggerAlert(QWidget *target) {
    if (!target) return;

    // ---- Blinking Effect ----
    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect(target);
    target->setGraphicsEffect(effect);
    effect->setColor(QColor("#ff0062"));
    effect->setStrength(0);

    QPropertyAnimation *colorAnim = new QPropertyAnimation(effect, "strength");
    colorAnim->setDuration(300);
    colorAnim->setStartValue(0.0);
    colorAnim->setKeyValueAt(0.5, 1.0);
    colorAnim->setEndValue(0.0);

    // ---- Shake Effect ----
    QPropertyAnimation *shakeAnim = new QPropertyAnimation(target, "pos");
    shakeAnim->setDuration(300);
    QPoint origPos = target->pos();
    int diff = 5;

    shakeAnim->setStartValue(origPos);
    shakeAnim->setKeyValueAt(0.1, origPos + QPoint(-diff, 0));
    shakeAnim->setKeyValueAt(0.3, origPos + QPoint(diff, 0));
    shakeAnim->setKeyValueAt(0.5, origPos + QPoint(-diff, 0));
    shakeAnim->setKeyValueAt(0.7, origPos + QPoint(diff, 0));
    shakeAnim->setKeyValueAt(0.9, origPos + QPoint(-diff, 0));
    shakeAnim->setEndValue(origPos);

    // ---- Run Together ----
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    group->addAnimation(colorAnim);
    group->addAnimation(shakeAnim);

    // ---- Cleanup ----
    connect(group, &QAnimationGroup::finished, [=]() {
        target->setGraphicsEffect(nullptr);
        group->deleteLater();
        activeTerminal->setFocus();
    });

    group->start();
}

void MainWindow::closeTerminal(QTermWidget *target) {
    if (!target) return;
    
    if (activeTerminals(rootSplitter) <= 1) {
        this->close();
        return;
    }

    QWidget *wrapperToDelete = target->parentWidget();
    QSplitter *splitter = qobject_cast<QSplitter*>(wrapperToDelete->parentWidget());

    if (activeTerminal == target) activeTerminal = nullptr;

    // Switch focus to immediate sibling
    bool focusFound = false;
    if (splitter && splitter->count() > 1) {
        int index = splitter->indexOf(wrapperToDelete);
        int siblingIndex = (index == 0) ? 1 : index - 1;
        
        if (auto sibWrapper = splitter->widget(siblingIndex)) {
            if (auto sibTerm = sibWrapper->findChild<QTermWidget*>()) {
                sibTerm->setFocus();
                focusFound = true;
            }
        }
    }

    // Fallback: Switch focus to a valid terminal
    if (!focusFound) {
        QList<QTermWidget*> allTerms = rootSplitter->findChildren<QTermWidget*>();
        for (auto t : allTerms) {
            if (t != target) {
                activeTerminal = t;
                activeTerminal->setFocus();
                break;
            }
        }
    }

    wrapperToDelete->deleteLater();

    // Cleanup redundant splitters
    QTimer::singleShot(0, this, [=]() {
        if (splitter && splitter->count() == 1 && splitter != rootSplitter) {
            QWidget *remaining = splitter->widget(0);
            QSplitter *gp = qobject_cast<QSplitter*>(splitter->parentWidget());
            if (gp) {
                int index = gp->indexOf(splitter);
                gp->insertWidget(index, remaining);
                splitter->deleteLater();
                
                QList<int> s;
                for(int i=0; i < gp->count(); ++i) s << 10000;
                gp->setSizes(s);
            }
        }
    });
}

void MainWindow::splitVertical() {
    replaceTerminalWithSplitter(Qt::Vertical);
}

void MainWindow::splitHorizontal() {
    replaceTerminalWithSplitter(Qt::Horizontal);
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

    if (ui->keyboardFrame->isVisible() && ui->keyboardFrame->height() >= targetKeyboardHeight) {
        // Hide
        keyboardIcon = QIcon(":/icons/assets/keyboard_s.svg");

        ui->keyboardFrame->setMinimumHeight(0);

        m_keyboardHeightAnimation->setStartValue(ui->keyboardFrame->height());
        m_keyboardHeightAnimation->setEndValue(0);
        m_keyboardHeightAnimation->start();

        QObject::connect(m_keyboardHeightAnimation, &QPropertyAnimation::finished, this, [this]() {
            ui->keyboardFrame->setVisible(false);
            ui->keyboardFrame->setMinimumHeight(targetKeyboardHeight);
            ui->keyboardFrame->setMaximumHeight(targetKeyboardHeight);
            ui->btnKeyboard->setToolTip("Show Keyboard");
        }, Qt::SingleShotConnection);
    } else {
        // Show
        keyboardIcon = QIcon(":/icons/assets/keyboard_h.svg");

        ui->keyboardFrame->setMaximumHeight(0);
        ui->keyboardFrame->setVisible(true);

        m_keyboardHeightAnimation->setStartValue(0);
        m_keyboardHeightAnimation->setEndValue(targetKeyboardHeight);
        m_keyboardHeightAnimation->start();

        QObject::connect(m_keyboardHeightAnimation, &QPropertyAnimation::finished, this, [this]() {
            ui->keyboardFrame->setMinimumHeight(targetKeyboardHeight);
            ui->keyboardFrame->setMaximumHeight(targetKeyboardHeight);
            ui->btnKeyboard->setToolTip("Hide Keyboard");
        }, Qt::SingleShotConnection);
    }

    if (dockKeyboard) {
        dockKeyboard->fadeIconSwap(keyboardIcon);
        dockKeyboard->animateCapsState(false);
    }
    
    activeTerminal->setFocus();
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
        
        activeTerminal->setFocus();
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

    activeTerminal->setFocus();    
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

    activeTerminal->sendText(keyValue);
    activeTerminal->setFocus();

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
        // Spawn Vertical Terminal
        if (letter == "b") {
            splitVertical();
            return QString();
        }
        // Spawn Horizontal Terminal
        if (letter == "h") {
            splitHorizontal();
            return QString();
        }
        // Open Media Explorer
        if (letter == "e") {
            openExplorer();
            return QString();
        }
        // Toggle Keyboard
        if (letter == "k") {
            onKeyboardClicked();
            return QString();
        }
        // Copy
        if (letter == "c") {
            activeTerminal->copyClipboard();
            return QString();
        }
        // Paste
        if (letter == "v") {
            activeTerminal->pasteClipboard();
            return QString();
        }
        // Close Terminal
        if (letter == "q" || letter == "w") {
            if (activeTerminal) closeTerminal(activeTerminal);
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

void MainWindow::keybindings() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Keybindings");
    dialog->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    dialog->setWindowOpacity(0.75);
    dialog->setFixedSize(360, 360);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QLabel *text = new QLabel();
    QString html = R"(
        <style>
            .header { font-size: 16px; }
            .key { font-family: monospace; color: white; }
            .desc { color: #99a0b2; padding-left: 9px; }
        </style>
        
        <div class='header'><b>APP</b></div>
        <table>
            <tr><td><span class='key'>Ctrl+Shift+B</span></td><td class='desc'>Spawn Vertical Terminal</td></tr>
            <tr><td><span class='key'>Ctrl+Shift+H</span></td><td class='desc'>Spawn Horizontal Terminal</td></tr>
            <tr><td></td><td class='desc'>(Limit: 16 Terminals)</td></tr>
            <tr><td><span class='key'>Ctrl+Shift+E</span></td><td class='desc'>Open Media Explorer</td></tr>
            <tr><td><span class='key'>Esc</span></td><td class='desc'>Close Media Explorer</td></tr>
            <tr><td><span class='key'>Ctrl+Shift+K</span></td><td class='desc'>Toggle Virtual Keyboard</td></tr>
            <tr><td><span class='key'>Ctrl+Shift+Q/W</span></td><td class='desc'>Close Active Terminal</td></tr>
            <tr></tr>
        </table>
        
        <div class='header'><b>MEDIA</b></div>
        <table>
            <tr><td><span class='key'>Space</span></td><td class='desc'>Play / Pause</td></tr>
            <tr><td><span class='key'>← / →</span></td><td class='desc'>Seek -/+ 10 seconds</td></tr>
            <tr><td><span class='key'>M</span></td><td class='desc'>Mute / Unmute Audio</td></tr>
            <tr><td><span class='key'>F</span></td><td class='desc'>Toggle Transparency (Video)</td></tr>
        </table>
    )";
    text->setText(html);
    layout->addWidget(text);
    dialog->exec();
    
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - dialog->width()) / 2;
    int y = (screenGeometry.height() - dialog->height()) / 2;
    dialog->move(x, y);
}

// ---- Screen/Keyboard Resizing
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    // ---- Update Keyboard ----    
    int currentScreenWidth = event->size().width();

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
    targetKeyboardHeight = newMaxHeight;
    
    ui->keyboardFrame->setMinimumSize(newMaxWidth, newMaxHeight);
    ui->keyboardFrame->setMaximumSize(newMaxWidth, newMaxHeight);
    ui->keyboardFrame->resize(newMaxWidth, newMaxHeight);
    ui->keyboardFrame->updateGeometry();
}

// ---- GUI Shortcuts ----
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    // ---- Focus Terminal ----
    if (event->type() == QEvent::FocusIn || event->type() == QEvent::MouseButtonPress) {
        // Gets direct QTermWidget or parent QTermWidget from QWidget child
        QTermWidget *term = qobject_cast<QTermWidget*>(obj);
        if (!term && obj->parent()) term = qobject_cast<QTermWidget*>(obj->parent());

        if (term && term != activeTerminal) {
            if (activeTerminal && activeTerminal->parentWidget()) {
                activeTerminal->parentWidget()->setProperty("focused", false);
                activeTerminal->parentWidget()->style()->unpolish(activeTerminal->parentWidget());
                activeTerminal->parentWidget()->style()->polish(activeTerminal->parentWidget());
            }

            activeTerminal = term;

            if (activeTerminal->parentWidget()) {
                activeTerminal->parentWidget()->setProperty("focused", true);
                activeTerminal->parentWidget()->style()->unpolish(activeTerminal->parentWidget());
                activeTerminal->parentWidget()->style()->polish(activeTerminal->parentWidget());
            }

            return true;
        }
    }

    // ---- Close Terminal Button ----
    if (event->type() == QEvent::ContextMenu) {
        QTermWidget *targetTerm = qobject_cast<QTermWidget*>(obj);
        if (!targetTerm && obj->parent()) targetTerm = qobject_cast<QTermWidget*>(obj->parent());

        if (targetTerm) {
            QMenu menu(this);
            QAction *closeAction = menu.addAction("Close Terminal");

            QPoint globalCenter = targetTerm->mapToGlobal(targetTerm->rect().center());
            QSize menuSize = menu.sizeHint();
            QPoint centeredPos(
                globalCenter.x() - (menuSize.width() / 2),
                globalCenter.y() - (menuSize.height() / 2)
            );

            QAction *selected = menu.exec(centeredPos);
            if (selected == closeAction) closeTerminal(targetTerm);

            return true;
        }
    }

    // ---- Keyboard Shortcuts ----
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Shift) onShiftPressed();
        if (keyEvent->key() == Qt::Key_Control) onCtrlPressed();

        if (isCtrl && isShift) {
            // Spawn Vertical Terminal
            if (keyEvent->key() == Qt::Key_B) {
                splitVertical();
                return true;
            }
            // Spawn Horizontal Terminal
            if (keyEvent->key() == Qt::Key_H) {
                splitHorizontal();
                return true;
            }
            // Open Media Explorer
            if (keyEvent->key() == Qt::Key_E) {
                openExplorer();
                return true;
            }
            // Toggle Keyboard
            if (keyEvent->key() == Qt::Key_K) {
                onKeyboardClicked();
                return true;
            }
            // Copy
            if (keyEvent->key() == Qt::Key_C) {
                activeTerminal->copyClipboard();
                return true;
            }
            // Paste
            if (keyEvent->key() == Qt::Key_V) {
                activeTerminal->pasteClipboard();
                return true;
            }
            // Close Terminal
            if (keyEvent->key() == Qt::Key_Q || keyEvent->key() == Qt::Key_W) {
                if (activeTerminal) closeTerminal(activeTerminal);
                return true;
            }
        }
    }

    // ---- Keyboard Release ----
    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        
        if (keyEvent->key() == Qt::Key_Shift) onShiftReleased();
        if (keyEvent->key() == Qt::Key_Control) onCtrlReleased();
    }

    return QMainWindow::eventFilter(obj, event);
}