#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include "pattern.h"

class MyHighlighter: QSyntaxHighlighter {
    Q_OBJECT
private:
    Pattern *myDict;
    QTextCharFormat wrongWordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat wrongGapFormat;
public:
    enum {T_OTHER, T_LETTER, T_NUMBER};
    MyHighlighter(Pattern *dict, QTextDocument *parent = nullptr);
protected:
    void highlightBlock(const QString& text) override;
    void highlightPart(const QString& text, int partType, int start, int end, bool isEnd);
};

class MyLineEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    MyLineEdit(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event);
private slots:
    void onTextChanged();
};

#endif // MYLINEEDIT_H
