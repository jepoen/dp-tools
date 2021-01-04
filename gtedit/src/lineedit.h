#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QDir>
#include <QFont>
#include <QMap>
#include <QString>
#include <QWidget>

class QLabel;
class QLayout;
class QLineEdit;

class LineEdit: public QWidget {
    Q_OBJECT
private:
    QLayout *myLayout;
    QLabel *lFile;
    QLabel *lImg;
    QLabel *myLineText;
    QLineEdit *myLineEdit;
    QDir myDir;
    QString myFileName;
    QMap<QString, bool> *myDict;
    bool myHasGt;
public:
    LineEdit(const QDir &dir, const QString& fileName, QMap<QString, bool> *dict, QWidget *parent = Q_NULLPTR);
    void setScale(double scale);
    void setFont(const QFont font);
    QLineEdit *editor() const { return myLineEdit; }
    QString handlePred(QString &text);
    void setHasGt(bool val) { myHasGt = val; }
    bool hasGt() const { return myHasGt; }
signals:
    void gtChanged();
private slots:
    void save();
    void del();
    void checkSpelling(const QString& text);
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
