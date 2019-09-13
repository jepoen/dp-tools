#include "textclassifier.h"

TextClassifier::TextClassifier()
{}

QList<Block> TextClassifier::classify(const QString &text) const {
    int state = C_OTHER;
    int oldState = C_OTHER;
    int iStart = 0;
    int iEnd = 0;
    QList<Block> blocks;
    for (int i = 0; i < text.size(); i++) {
        if (text.at(i).isLetter() || text.at(i) == '\'') state = C_LETTER;
        else if (text.at(i).isNumber()) state = C_NUMBER;
        else state = C_OTHER;
        if (state == oldState) {
            iEnd = i;
        } else {
            blocks.append(Block(iStart, i-iStart, oldState, text.mid(iStart, i-iStart)));
            iStart = iEnd = i;
            oldState = state;
        }
    }
    blocks.append(Block(iStart, text.size()-iStart, oldState, text.mid(iStart)));
    return blocks;
}
