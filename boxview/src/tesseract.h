#ifndef TESSERACT_H
#define TESSERACT_H

#include <QObject>
#include <QString>

class Tesseract: public QObject
{
    Q_OBJECT
private:
    QString myTesseractPath;
    QString myTesseractModel;
public:
    Tesseract(QObject *parent);
    Tesseract(const QString& tessPath, const QString& model, QObject *parent):
        QObject(parent),
        myTesseractPath(tessPath), myTesseractModel(model) {}
    void setTesseractPath(const QString& tessPath) { myTesseractPath = tessPath; }
    void setTesseractModel(const QString& model) { myTesseractModel = model; }
    QString buildBoxFile(const QString& imgFile) const;
    QString buildTrainFile(const QString& boxFile, const QString& imgFile);
};

#endif // TESSERACT_H
