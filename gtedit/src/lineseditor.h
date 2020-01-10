#ifndef LINESEDITOR_H
#define LINESEDITOR_H

#include <QDir>
#include <QDialog>
#include <QStringList>
class QLabel;
class QLineEdit;

class LinesEditor : public QDialog {
private:
    QDir dir;
    QStringList fileNames;
    QList<QLineEdit *> lines;
public:
    LinesEditor();
};

#endif // LINESEDITOR_H
