#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QDir>
#include <QFont>
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
    QLineEdit *myLineEdit;
    QDir myDir;
    QString myFileName;

public:
    LineEdit(const QDir &dir, const QString& fileName, QWidget *parent = Q_NULLPTR);
    void setScale(double scale);
    void setFont(const QFont font);
    QLineEdit *editor() const { return myLineEdit; }
private slots:
    void save();
    void del();
};



#endif // LINEEDIT_H
