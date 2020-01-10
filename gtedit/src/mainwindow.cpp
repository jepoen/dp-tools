#include <QtGui>
#include <QtWidgets>
#include "mainwindow.h"
#include "lineedit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createMenus();
    createMainWidget();
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
    }
    widget->setLayout(layout);
    mainWidget->setWidget(widget);
}

void MainWindow::openPage() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("line directory"));
    showPage(dir);
}

void MainWindow::handleLine(QAction *action) {
    qDebug()<<"handleLine"<<action->data();
}
