#include <QtGui>
#include <QtWidgets>
#include "mainwindow.h"
#include "lineedit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    readDict();
    myDir = QDir(".").absolutePath();
    myScale = 1.0;
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
    QFont font("DPSansMono", 14);
    QDir dir = QDir(dirName);
    QStringList fileNames = dir.entryList(QStringList()<<"l-???.png", QDir::Files, QDir::Name);

    for (QString f: fileNames) {
        LineEdit *le = new LineEdit(dir, f, myDict);
        le->setFont(font);
        le->setScale(myScale);
        layout->addWidget(le);
        myLineEdits.append(le);
        connect(le->editor(), SIGNAL(cursorPositionChanged(int, int)), this, SLOT(getCurrentChar(int, int)));
        connect(le, &LineEdit::gtChanged, this, &MainWindow::countGtLines);
    }
    if (myLineEdits.size() > 0) {
        qDebug()<<"Set focus";
        QTimer::singleShot(0, myLineEdits.at(0)->editor(), SLOT(setFocus()));
        myLineEdits.at(0)->editor()->home(false);
        QTimer::singleShot(0, this, SLOT(getCurrentChar(0, 0)));
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

void MainWindow::getCurrentChar(int /*oldPos*/, int newPos) {
    QWidget *currWidget =  focusWidget();
    QLineEdit *ed = qobject_cast<QLineEdit*>(currWidget);
    qDebug()<<"Cursor changed";
    if (ed == nullptr) return;
    if (newPos >= ed->text().size()) {
        qDebug()<<"pos"<<newPos<<" size "<<ed->text().size();
        return;
    }
    QChar c = ed->text().at(newPos);
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
    myScale = action->data().toInt()*0.01;
    for(LineEdit *ed: myLineEdits) {
        ed->setScale(myScale);
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
    myDict = new Pattern(words, gaps);
    qDebug()<<"dictionary: "<<myDict->wordSize()<<" "<<myDict->gapSize();
    myDict->showGaps();
}
