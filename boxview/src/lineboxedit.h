#ifndef LINEBOXEDIT_H
#define LINEBOXEDIT_H

#include <QList>
#include <QRect>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include<QTextEdit>

class Dictionary;

class BlockHighlighter: public QSyntaxHighlighter {
    Q_OBJECT
private:

    QRegularExpression myGapExp;
    Dictionary *myDict;
    QTextCharFormat myWordFormat;
    QTextCharFormat myLongSFormat;
    QTextCharFormat myGapFormat;
    enum {C_OTHER, C_LETTER, C_NUMBER, C_UNKNOWN_WORD};

    bool isUnusualGap(const QString& text) const;

public:
    BlockHighlighter(Dictionary *dict, QTextDocument *parent=nullptr);
protected:
    void highlightBlock(const QString &text) override;
};

class LineboxEdit : public QTextEdit
{
        Q_OBJECT
private:
    QString myFileName;
    QList<QRect> myBoxes;
    QList<QString> myLines;
    BlockHighlighter *myHighlighter;
    Dictionary *myDict;
    int myCurrentLine;
    void handleFrac();
public:
    LineboxEdit(Dictionary *dict, QWidget *parent=nullptr);

    void readFile(const QString& fileName);
    void writeFile(const QString& fileName);

signals:
    void lineChanged(const QRect&);

private slots:
    void onCursorPosition();
};

#endif // LINEBOXEDIT_H
