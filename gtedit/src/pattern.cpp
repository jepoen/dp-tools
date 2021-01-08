#include <QtDebug>
#include "pattern.h"

void Pattern::showGaps() const {
    qDebug()<<"Gaps:";
    for (const QString& gap: myGaps) {
        qDebug()<<QString("[%1]").arg(gap);
    }
}
