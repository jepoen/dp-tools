#include <QtGui>
#include <QtWidgets>
#include <QtDebug>
#include "lineedit.h"

LineEdit::LineEdit(const QDir& dir, const QString& fileName, QWidget *parent):
    QWidget(parent),
    myDir(dir), myFileName(fileName)
{
    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);
    QString baseName = QFileInfo(fileName).baseName();
    qDebug()<<fileName<<QFileInfo(fileName).baseName();
    QFileInfo gtFile(dir, baseName+".gt.txt");
    QFileInfo predFile(dir, baseName+".pred.txt");
    qDebug()<<"gt"<<gtFile<<"pred"<<predFile;
    QFile file;
    bool hasGt = false;
    bool hasPred = false;
    QPalette paletteEmpty;
    QString StyleGt = "QLabel {background-color: green;}";
    QString StylePred = "QLabel {background-color: yellow;}";
    if (predFile.exists()) {
        hasPred = true;
        qDebug()<<predFile;
        file.setFileName(predFile.absoluteFilePath());
    }
    if (gtFile.exists()) {
        hasGt = true;
        qDebug()<<gtFile;
        file.setFileName(gtFile.absoluteFilePath());
    }

    QHBoxLayout *lControl = new QHBoxLayout();
    lFile = new QLabel(baseName);
    if (hasGt) {
        lFile->setStyleSheet(StyleGt);
    } else if (hasPred) {
        lFile->setStyleSheet(StylePred);
    }
    lControl->addWidget(lFile);
    QToolButton *bSave = new QToolButton();
    bSave->setFocusPolicy(Qt::NoFocus);
    QAction *saveAction = new QAction(tr("Save"), this);
    bSave->setDefaultAction(saveAction);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    lControl->addWidget(bSave);
    QToolButton *bDel = new QToolButton();
    bDel->setFocusPolicy(Qt::NoFocus);
    QAction *delAction = new QAction(tr("Delete"), this);
    bDel->setDefaultAction(delAction);
    connect(delAction, SIGNAL(triggered()), this, SLOT(del()));
    lControl->addWidget(bDel);
    lControl->addStretch();
    layout->addLayout(lControl);

    QFileInfo imgFile = QFileInfo(dir, fileName);
    qDebug()<<imgFile.absoluteFilePath();
    lImg = new QLabel();
    QPixmap pixmap(imgFile.absoluteFilePath());

    lImg->setPixmap(pixmap);
    layout->addWidget(lImg);
    myLineEdit = new QLineEdit();
    file.open(QFile::ReadOnly|QFile::Text);
    myLineEdit->setText(QString(file.readAll()));
    myLineEdit->setCursorPosition(0);
    layout->addWidget(myLineEdit);
    qDebug()<<"widget created";
}

void LineEdit::setScale(double scale) {
    QFileInfo imgFile = QFileInfo(myDir, myFileName);
    QPixmap pixmap(imgFile.absoluteFilePath());
    int w = int(pixmap.width()*scale);
    int h = int(pixmap.height()*scale);
    lImg->setPixmap(pixmap.scaled(w, h));
}

void LineEdit::setFont(const QFont font) {
    myLineEdit->setFont(font);
}

void LineEdit::save() {
    QString baseName = QFileInfo(myFileName).baseName();
    QFileInfo gtFile(myDir, baseName+".gt.txt");
    qDebug()<<"save gt"<<gtFile;
    QFile fo(gtFile.absoluteFilePath());
    fo.open(QFile::WriteOnly|QFile::Text);
    QTextStream out(&fo);
    out<<myLineEdit->text();
    fo.close();
    QString StyleGt = "QLabel {background-color: green;}";
    lFile->setStyleSheet(StyleGt);
}

void LineEdit::del() {
    QString baseName = QFileInfo(myFileName).baseName();
    QFileInfo gtFile(myDir, baseName+".gt.txt");
    QFileInfo predFile(myDir, baseName+".pred.txt");
    QFileInfo delFile(myDir, baseName+".unused.txt");
    QFile fGt(gtFile.absoluteFilePath());
    fGt.remove();
    QFile fPred(predFile.absoluteFilePath());
    fPred.rename(delFile.absoluteFilePath());
    lFile->setStyleSheet("");
}
