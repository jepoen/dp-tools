#include <QtWidgets>

#include "mainwindow.h"

#include "dictionary.h"
#include "lineboxedit.h"
#include "page.h"

// evt. paket qt5-image-formats-plugins erforderlich
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QSplitter *splitter = new QSplitter();
    myDict = new Dictionary("words-1901.txt");
    myFilesWidget = new QListWidget();
    myEdit = new LineboxEdit(myDict);
    myPage = new Page();
    setCentralWidget(splitter);
    splitter->addWidget(myFilesWidget);
    splitter->addWidget(myEdit);
    splitter->addWidget(myPage);
    splitter->setSizes(QList<int>()<<150<<1200<<600);
    createActions();
    createMenu();
    loadSettings();
    readDir(".");
    resize(1800, 800);
    qDebug()<<QImageReader::supportedImageFormats();
    qDebug()<<mytesseractDir;
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions() {
    openAction = new QAction(tr("Open Dir ..."), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    saveAction = new QAction(tr("Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    updateDistAction = new QAction(tr("Update distance"));
    connect(updateDistAction, SIGNAL(triggered()), myEdit, SLOT(updateDist()));
    connect(myFilesWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(openFileItem(QListWidgetItem*)));
    connect(myEdit, SIGNAL(lineChanged(const QRect&)), myPage, SLOT(changeLineBox(const QRect&)));
    connect(myEdit, SIGNAL(cursorPositionChanged()), this, SLOT(changeEditPos()));
}

void MainWindow::createMenu() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(updateDistAction);
}

void MainWindow::closeEvent(QCloseEvent */*evt*/) {
    qDebug()<<"closeEvent";
    saveSettings();
}

void MainWindow::loadSettings() {
    QSettings settings;
    mytesseractDir = settings.value("tesseractDir", "/opt/tesseract/bin").toString();
}

void MainWindow::saveSettings() {
    QSettings settings;
    settings.setValue("tesseractDir", mytesseractDir);
}

void MainWindow::showPage(const QString &baseName) {
    //QString boxFileName = baseName+".box";
    myCurrentFile = baseName;
    qDebug()<<"showPage "<<baseName;
    QString pixFileName = baseName+".tif";
    if (QFileInfo::exists(pixFileName)) {
        QPixmap pixmap(pixFileName);
    // set page first
        qDebug()<<"pixmap "<<pixFileName<<" "<<pixmap.size();
        myPage->setPage(pixmap);
    }
    myEdit->readFile(baseName);
}

void MainWindow::open() {
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Scans directory"), QString(), QFileDialog::ShowDirsOnly);
    if (dirName.isEmpty()) return;
    myCurrentDir = dirName;
    readDir(dirName);
}

void MainWindow::save() {
    myEdit->writeFile(myCurrentFile+".box");
}

void MainWindow::readDir(const QString &dirName) {
    QDir dir(dirName);
    QStringList fileNames = dir.entryList(QStringList()<<"*.tif", QDir::Files, QDir::Name);
    qDebug()<<fileNames;
    myFilesWidget->clear();
    for (const QString& fileName: fileNames) {
        myFilesWidget->addItem(new QListWidgetItem(fileName));
    }
    if (myFilesWidget->count() > 0) {
        openFileItem(myFilesWidget->item(0));
    }
}

void MainWindow::openFileItem(QListWidgetItem *item) {
    QString fileName = item->text();
    QString baseName = QFileInfo(fileName).baseName();
    QString filePath = QDir(myCurrentDir).absoluteFilePath(baseName);
    showPage(filePath);
}

void MainWindow::changeEditPos() {
    QChar currentChar = myEdit->currentChar();
    qDebug()<<currentChar;
}
