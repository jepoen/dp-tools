#ifndef TEXTCLASSIFIER_H
#define TEXTCLASSIFIER_H

#include <QList>
#include <QString>

class Block {
private:
    int myStart;
    int myLen;
    int myType;
    QString myText;
public:
    Block(int start, int len, int type, const QString& text):
        myStart(start), myLen(len), myType(type), myText(text) {}
    int start() const { return myStart; }
    int len() const { return myLen; }
    int type() const { return myType; }
    QString text()  const { return myText; }
};

class TextClassifier
{
public:
    enum {C_OTHER, C_LETTER, C_NUMBER};

    TextClassifier();
    QList<Block> classify(const QString& text) const;
};

#endif // TEXTCLASSIFIER_H
