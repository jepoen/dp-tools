#include <QtDebug>
#include "pattern.h"

void Pattern::showGaps() const {
    qDebug()<<"Gaps:";
    for (const QString& gap: myGaps) {
        qDebug()<<QString("[%1]").arg(gap);
    }
}

bool Pattern::containsTrWord(QString word) const {
    QString repl = word.replace(QChar(0x17f), QChar('s'));
    //qDebug()<<"containsTrWord"<<word<<repl;
    return myWords.contains(repl);
}
