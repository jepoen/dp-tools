#include <QtGui>
#include <QtWidgets>
#include "mainwindow.h"
#include "lineedit.h"
#include "mylineedit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readDict();
    readSettings();
    myDir = QDir(".").absolutePath();
    createActions();
    createMenus();
    createMainWidget();
    createStatusBar();
    resize(1000, 800);

}

MainWindow::~MainWindow() {
}

void MainWindow::createActions() {
    openAction = new QAction(tr("Open page"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(openPage()));
    saveSettingsAction = new QAction(tr("Save settings"), this);
    connect(saveSettingsAction, &QAction::triggered, this, &MainWindow::saveSettings);
    selectFontAction = new QAction(tr("Select font …"), this);
    connect(selectFontAction, &QAction::triggered, this, &MainWindow::selectFont);
    quitAction = new QAction(tr("Quit"), this);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QApplication::quit);
    scale50Action = new QAction(tr("50%"), this);
    scale50Action->setShortcut(Qt::CTRL+Qt::Key_5);
    scale50Action->setData(50);
    connect(scale50Action, SIGNAL(triggered()), this, SLOT(setScale()));
    scale100Action = new QAction(tr("100%"), this);
    scale100Action->setShortcut(Qt::CTRL+Qt::Key_1);
    scale100Action->setData(100);
    connect(scale100Action, SIGNAL(triggered()), this, SLOT(setScale()));
}

void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(selectFontAction);
    fileMenu->addAction(saveSettingsAction);
    fileMenu->addAction(quitAction);
    QMenu *viewMenu = menuBar()->addMenu(("&View"));
    viewMenu->addAction(scale50Action);
    viewMenu->addAction(scale100Action);
}

void MainWindow::createMainWidget() {
    mainWidget = new QScrollArea();
    mainWidget->setWidgetResizable(true);
    editActions = new QActionGroup(mainWidget);
    connect(editActions, SIGNAL(triggered(QAction *)), this, SLOT(handleLine(QAction *)));
    setCentralWidget(mainWidget);
}

void MainWindow::createStatusBar() {
    lCurrentChar = new QLabel();
    statusBar()->addWidget(lCurrentChar);
    lGtLines = new QLabel();
    statusBar()->addWidget(lGtLines);
}

void MainWindow::showPage(const QString& dirName) {
    //LinesEditor editor;
    //editor.exec();
    setWindowTitle(dirName);
    for (QAction *action: editActions->actions()) {
        editActions->removeAction(action);
        action->deleteLater();
    }
    myLineEdits.clear();
    QWidget *widget = mainWidget->widget();
    if (widget) {
        widget->deleteLater();
    }
    widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    QFont font(mySettings.fontName(), mySettings.fontSize());
    QDir dir = QDir(dirName);
    QStringList fileNames = dir.entryList(QStringList()<<"l-???.png", QDir::Files, QDir::Name);

    for (QString f: fileNames) {
        LineEdit *le = new LineEdit(dir, f, myDict);
        le->setFont(font);
        le->setScale(mySettings.pixelScale());
        layout->addWidget(le);
        myLineEdits.append(le);
        connect(le->editor(), SIGNAL(cursorPositionChanged()), this, SLOT(getCurrentChar()));
        connect(le, &LineEdit::gtChanged, this, &MainWindow::countGtLines);
    }
    if (myLineEdits.size() > 0) {
        qDebug()<<"Set focus";
        QTimer::singleShot(0, myLineEdits.at(0), SLOT(setFocus()));
        myLineEdits.at(0)->editor()->moveCursor(QTextCursor::Start);
        QTimer::singleShot(0, this, SLOT(getCurrentChar()));
    }
    widget->setLayout(layout);
    mainWidget->setWidget(widget);
    countGtLines();
}

void MainWindow::openPage() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("line directory"), myDir);
    if (!dir.isEmpty()) {
        myDir = dir;
        showPage(dir);
    }
}

void MainWindow::handleLine(QAction *action) {
    qDebug()<<"handleLine"<<action->data();
}

void MainWindow::getCurrentChar() {
    QWidget *currWidget =  focusWidget();
    MyLineEdit *ed = qobject_cast<MyLineEdit*>(currWidget);
    if (ed == nullptr) return;
    int newPos = ed->textCursor().position();
    qDebug()<<"Cursor changed";
    if (newPos >= ed->toPlainText().size()) {
        qDebug()<<"pos"<<newPos<<" size "<<ed->toPlainText().size();
        return;
    }
    QChar c = ed->toPlainText().at(newPos);
    qDebug()<<c;
    lCurrentChar->setText(QString("%1 %2").arg(c).arg(c.unicode(), 4, 16));
}

void MainWindow::countGtLines() {
    int count = 0;
    for (const LineEdit* le: myLineEdits) {
        if (le->hasGt()) count++;
    }
    lGtLines->setText(tr("GT lines: %1").arg(count));
}

void MainWindow::setScale() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action == nullptr) return;
    mySettings.setPixelScale(action->data().toInt()*0.01);
    for(LineEdit *ed: myLineEdits) {
        ed->setScale(mySettings.pixelScale());
    }
}

void MainWindow::readDict() {
    QSet<QString> words;
    QFile fi(":/resources/dict-de.dat");
    if (fi.open(QFile::ReadOnly)) {
        QTextStream in(&fi);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(" ");
            for (const QString& part: parts) {
                words.insert(part);
            }
        }
    }
    QSet<QString> gaps;
    QFile fiGaps(":/resources/gaps.dat");
    if (fiGaps.open(QFile::ReadOnly)) {
        QTextStream in(&fiGaps);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            QString gap = line.mid(1, line.size()-2);
            gaps.insert(gap);
        }
    }
    QSet<QString> endGaps;
    QFile fiEndgaps(":/resources/endgaps.dat");
    if (fiEndgaps.open(QFile::ReadOnly)) {
        QTextStream in(&fiEndgaps);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            QString gap = line.mid(1, line.size()-2);
            endGaps.insert(gap);
        }
    }
    myDict = new Pattern(words, gaps, endGaps);
    qDebug()<<"dictionary: "<<myDict->wordSize()<<" "<<myDict->gapSize();
    myDict->showGaps();
}

void MainWindow::selectFont() {
    bool ok;
    QFont font = QFontDialog::getFont(
                &ok,
                QFont(mySettings.fontName(), mySettings.fontSize()),
                this
    );
    if (ok) {
        mySettings.setFontName(font.family());
        mySettings.setFontSize(font.pointSize());
    }
}

void MainWindow::readSettings() {
    QSettings settings("dp-tools", "gtedit");
    double pixelScale = settings.value("pixelScale", "1.0").toDouble();
    QString fontName = settings.value("fontName", "DP Sans Mono").toString();
    int fontSize = settings.value("fontSize", "14").toInt();
    mySettings.setPixelScale(pixelScale);
    mySettings.setFontName(fontName);
    mySettings.setFontSize(fontSize);
}

void MainWindow::saveSettings() {
    QSettings settings("dp-tools", "gtedit");
    settings.setValue("pixelScale", mySettings.pixelScale());
    settings.setValue("fontName", mySettings.fontName());
    settings.setValue("fontSize", mySettings.fontSize());
}
