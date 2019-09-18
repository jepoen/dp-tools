#include <QtWidgets>

#include "mainwindow.h"

#include "dictionary.h"
#include "lineboxedit.h"
#include "page.h"
#include "tesseract.h"

// evt. paket qt5-image-formats-plugins erforderlich
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    loadSettings();
    QSplitter *splitter = new QSplitter();
    myDict = new Dictionary("words-1901.txt");
    myFilesWidget = new QListWidget();
    myEdit = new LineboxEdit(myDict);
    myPage = new Page();
    tesseract = new Tesseract(myTesseractPath, myTesseractModel, this);
    setCentralWidget(splitter);
    splitter->addWidget(myFilesWidget);
    splitter->addWidget(myEdit);
    splitter->addWidget(myPage);
    splitter->setSizes(QList<int>()<<150<<1200<<600);
    createActions();
    createMenu();
    readDir(".");
    resize(1800, 800);
    qDebug()<<QImageReader::supportedImageFormats();
    qDebug()<<myTesseractPath;
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
    deleteLineAction = new QAction(tr("Delete current line and box"), this);
    connect(deleteLineAction, SIGNAL(triggered()), myEdit, SLOT(deleteLine()));
    updateDistAction = new QAction(tr("Update distance"), this);
    connect(updateDistAction, SIGNAL(triggered()), myEdit, SLOT(updateDist()));
    replaceFromProofedAction = new QAction(tr("Replace by proofed text"), this);
    connect(replaceFromProofedAction, SIGNAL(triggered()), myEdit, SLOT(replaceFromProofed()));
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
    editMenu->addAction(replaceFromProofedAction);
    editMenu->addAction(deleteLineAction);
}

void MainWindow::closeEvent(QCloseEvent */*evt*/) {
    qDebug()<<"closeEvent";
    saveSettings();
}

void MainWindow::loadSettings() {
    QSettings settings;
    myTesseractPath = settings.value("tesseractPath", "/opt/tesseract/bin/tesseract").toString();
    myTesseractModel = settings.value("tesseractModel", "deu_frak").toString();
}

void MainWindow::saveSettings() {
    QSettings settings;
    settings.setValue("tesseractPath", myTesseractPath);
    settings.setValue("tesseractModel", myTesseractModel);

}

void MainWindow::showPage(const QString &pixFileName) {
    //QString boxFileName = baseName+".box";
    QString baseName = QFileInfo(pixFileName).absoluteFilePath();
    baseName = baseName.left(baseName.lastIndexOf('.'));
    myCurrentFile = baseName;
    qDebug()<<"showPage "<<baseName<<pixFileName;
    if (QFileInfo::exists(pixFileName)) {
        QPixmap pixmap(pixFileName);
    // set page first
        qDebug()<<"pixmap "<<pixFileName<<" "<<pixmap.size();
        myPage->setPage(pixmap);
    }
    QString rawBoxName = baseName+"-raw.box";
    qDebug()<<"raw box name"<<rawBoxName;
    if (!QFileInfo::exists(rawBoxName)) {
        qDebug()<<"build "<<rawBoxName;
        tesseract->buildBoxFile(pixFileName);
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
    QStringList fileNames = dir.entryList(QStringList()<<"*.tif"<<"*png", QDir::Files, QDir::Name);
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
    QString filePath = QDir(myCurrentDir).absoluteFilePath(fileName);
    showPage(filePath);
}

void MainWindow::changeEditPos() {
    QChar currentChar = myEdit->currentChar();
    qDebug()<<currentChar;
}
