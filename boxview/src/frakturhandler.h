#ifndef FRAKTURHANDLER_H
#define FRAKTURHANDLER_H

#include <QRegularExpression>
#include <QString>
#include "textclassifier.h"

class FrakturHandler
{
private:
    QRegularExpression schExpr;
    QRegularExpression schaftExpr;
    TextClassifier myClassifier;
    QString handleSch(QString line) const;
    QString handleI(QString line) const;
    QString handleStartS(QString line) const;
    QString handleQ(QString line) const;
public:
    FrakturHandler();
    QString handle(QString line) const;
};

#endif // FRAKTURHANDLER_H
