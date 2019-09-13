#include <QtWidgets>

#include "mainwindow.h"

#include "dictionary.h"
#include "lineboxedit.h"
#include "page.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QSplitter *splitter = new QSplitter();
    myDict = new Dictionary("words-1901.txt");
    myFilesWidget = new QListWidget();
    myEdit = new LineboxEdit(myDict);
    myPage = new Page();
    connect(myEdit, SIGNAL(lineChanged(const QRect&)), myPage, SLOT(changeLineBox(const QRect&)));
    setCentralWidget(splitter);
    splitter->addWidget(myFilesWidget);
    splitter->addWidget(myEdit);
    splitter->addWidget(myPage);
    splitter->setSizes(QList<int>()<<400<<1200<<600);
    createActions();
    createMenu();
    showPage("015");
    resize(1400,800);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions() {
    openAction = new QAction(tr("Open Dir ..."), this);
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(myFilesWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(openFileItem(QListWidgetItem*)));
}

void MainWindow::createMenu() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
}

void MainWindow::showPage(const QString &baseName) {
    //QString boxFileName = baseName+".box";
    QString pixFileName = baseName+".tif";
    QPixmap pixmap(pixFileName);
    // set page first
    myPage->setPage(pixmap);
    myEdit->readFile(baseName);
}

void MainWindow::open() {
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Scans directory"), QString(), QFileDialog::ShowDirsOnly);
    if (dirName.isEmpty()) return;
    myCurrentDir = dirName;
    readDir(dirName);
}

void MainWindow::save() {

}

void MainWindow::readDir(const QString &dirName) {
    QDir dir(dirName);
    QStringList fileNames = dir.entryList(QStringList()<<"*.tif", QDir::Files, QDir::Name);
    qDebug()<<fileNames;
    myFilesWidget->clear();
    for (const QString& fileName: fileNames) {
        myFilesWidget->addItem(new QListWidgetItem(fileName));
    }
}

void MainWindow::openFileItem(QListWidgetItem *item) {
    QString fileName = item->text();
    QString baseName = QFileInfo(fileName).baseName();
    QString filePath = QDir(myCurrentDir).absoluteFilePath(baseName);
    showPage(filePath);
}
