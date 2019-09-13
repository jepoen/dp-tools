#include <QStringLiteral>
#include <QtDebug>

#include "frakturhandler.h"

FrakturHandler::FrakturHandler() {
    schExpr = QRegularExpression(QStringLiteral("^[fs]ch|[fs]ch$"));
    schaftExpr = QRegularExpression(QStringLiteral("[fs]chaft$"));
}

QString FrakturHandler::handle(QString line) const {
    qDebug()<<"handle "<<line;
    QList<Block> blocks = myClassifier.classify(line);
    QString newLine;
    for (const Block& block: blocks) {
        QString word = block.text();
        if (block.type() == TextClassifier::C_LETTER) {
            word = handleSch(word);
            qDebug()<<"sch "<<word;
            word = handleI(word);
            qDebug()<<"I "<<word;
        }
        newLine.append(word);
    }
    return newLine;
}

QString FrakturHandler::handleI(QString line) const {
    QList<QChar> vocals;
    vocals<<'a'<<'e'<<'i'<<'o'<<'u'<<'y';
    if (line[0] == 'I' || line[0] == 'J') {
        if (vocals.contains(line[1])) {
            line[0] = 'J';
        } else {
            line[0] = 'I';
        }
    }
    return line;
}

QString FrakturHandler::handleSch(QString line) const {
    line.replace(schExpr, QStringLiteral("ſch"));
    line.replace(schaftExpr, QStringLiteral("ſchaft"));
    return line;
}
