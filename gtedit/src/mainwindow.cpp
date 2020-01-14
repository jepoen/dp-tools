#include <QtGui>
#include <QtWidgets>
#include "mainwindow.h"
#include "lineedit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
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
    connect(openAction, SIGNAL(triggered()), this, SLOT(openPage()));
}

void MainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
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
    QFont font("DPCustomMono2", 14);
    QDir dir = QDir(dirName);
    QStringList fileNames = dir.entryList(QStringList()<<"l-???.png", QDir::Files, QDir::Name);

    for (QString f: fileNames) {
        LineEdit *le = new LineEdit(dir, f);
        le->setFont(font);
        le->setScale(0.5);
        layout->addWidget(le);
        myLineEdits.append(le);
        connect(le->editor(), SIGNAL(cursorPositionChanged(int, int)), this, SLOT(getCurrentChar(int, int)));
    }
    if (myLineEdits.size() > 0) {
        qDebug()<<"Set focus";
        QTimer::singleShot(0, myLineEdits.at(0)->editor(), SLOT(setFocus()));
        myLineEdits.at(0)->editor()->home(false);
        QTimer::singleShot(0, this, SLOT(getCurrentChar(0, 0)));
    }
    widget->setLayout(layout);
    mainWidget->setWidget(widget);
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
