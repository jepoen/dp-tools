#include "tesseract.h"
#include <QtDebug>
#include <QFileInfo>
#include <QProcess>

Tesseract::Tesseract(QObject *parent): QObject(parent), myTesseractPath(QString()), myTesseractModel(QString())
{}

QString Tesseract::buildBoxFile(const QString &imgFile) const {
    QProcess process;
    QStringList args;
    QString baseName = imgFile.left(imgFile.lastIndexOf('.'));
    QString boxName = baseName + "-raw";
    args<<imgFile<<boxName<<"-l"<<myTesseractModel<<"lstmbox";
    qDebug()<<myTesseractPath<<" "<<args;
    process.start(myTesseractPath, args);
    process.waitForFinished();
    return boxName + ".box";
}

QString Tesseract::buildTrainFile(const QString &boxFile, const QString &imgFile) {
    // TODO
    return "";
}
