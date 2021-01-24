#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QDir>
#include <QFont>
#include <QMap>
#include <QString>
#include <QWidget>
#include "pattern.h"

class QLabel;
class QLayout;
class MyLineEdit;
class MyHighlighter;

class LineEdit: public QWidget {
    Q_OBJECT
private:
    QLayout *myLayout;
    QLabel *lFile;
    QLabel *lImg;
    MyLineEdit *myLineEdit;
    MyHighlighter *myHighlighter;
    QDir myDir;
    QString myFileName;
    Pattern *myDict;
    bool myHasGt;
public:
    LineEdit(const QDir &dir, const QString& fileName, Pattern *dict, QWidget *parent = Q_NULLPTR);
    void setScale(double scale);
    void setFont(const QFont font);
    MyLineEdit *editor() const { return myLineEdit; }
    QString handlePred(QString &text);
    void setHasGt(bool val) { myHasGt = val; }
    bool hasGt() const { return myHasGt; }
signals:
    void gtChanged();
private slots:
    void save();
    void del();
};

class LinePart {
private:
    QString myText;
    int myType;
public:
    enum {TEXT, NUMBER, OTHER};
    LinePart(const QString& text, int type):
        myText(text), myType(type)
    {}
    QString text() const { return myText; }
    int type() const { return myType; }
};

#endif // LINEEDIT_H
