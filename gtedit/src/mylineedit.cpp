#include <QtDebug>
#include <QString>
#include "mylineedit.h"

MyHighlighter::MyHighlighter(Pattern *dict, QTextDocument *parent):
    QSyntaxHighlighter(parent), myDict(dict) {
    wrongWordFormat.setForeground(Qt::red);
    wrongWordFormat.setFontWeight(QFont::Bold);
    wrongGapFormat.setBackground(Qt::yellow);
    wrongGapFormat.setFontWeight(QFont::Bold);
    numberFormat.setFontItalic(true);
    numberFormat.setForeground(Qt::darkBlue);
}

void MyHighlighter::highlightBlock(const QString &text) {
    int partType = T_OTHER;
    int oldPos = 0;
    int pos = 0;
    for (; pos < text.size(); pos++) {
        QChar c = text.at(pos);
        int pt = partType;
        if (c.isLetter()) {
            pt = T_LETTER;
        } else if (c.isDigit()) {
            pt = T_NUMBER;
        } else {
            pt = T_OTHER;
        }
        if (pt != partType) {
            if (pos-oldPos > 0) {
                highlightPart(text, partType, oldPos, pos-oldPos, false);
            }
            partType = pt;
            oldPos = pos;
        }
    }
    if (pos-oldPos > 0) {
        highlightPart(text, partType, oldPos, pos-oldPos, true);
    }
}

void MyHighlighter::highlightPart(const QString &text, int partType, int start, int len, bool isEnd) {
    QString part = text.mid(start, len);
    qDebug()<<"highlightPart"<<part<<text;
    switch (partType) {
    case T_LETTER:
        if (!myDict->containsWord(part)) {
            setFormat(start, len, wrongWordFormat);
        }
        return;
     case T_NUMBER:
        setFormat(start, len, numberFormat);
        return;
     default: // Gap
        if (isEnd) {
            if (!myDict->containsEndGap(part)) {
                setFormat(start, len, wrongGapFormat);
            }
        } else {
            if (!myDict->containsGap(part)) {
                setFormat(start, len, wrongGapFormat);
            }
        }
        return;
    }
}

MyLineEdit::MyLineEdit(QWidget *parent):
    QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(1);
    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

void MyLineEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        return;
    }
    QPlainTextEdit::keyPressEvent(event);
}

void MyLineEdit::onTextChanged() {
    //qDebug()<<"onTextChanged"<<toPlainText();
    QFontMetrics fm(font());
    setFixedHeight(5*fm.xHeight());
}
