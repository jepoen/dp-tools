#ifndef LINEBOXEDIT_H
#define LINEBOXEDIT_H

#include <QList>
#include <QRect>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include<QTextEdit>

#include "textdistance.h"

class Dictionary;
class LineboxEdit;

class BlockHighlighter: public QSyntaxHighlighter {
    Q_OBJECT
private:
    LineboxEdit *myEditor;
    QRegularExpression myGapExp;
    Dictionary *myDict;
    QTextCharFormat myWordFormat;
    QTextCharFormat myLongSFormat;
    QTextCharFormat myGapFormat;
    QTextCharFormat mySFormat;

    bool isUnusualGap(const QString& text) const;

public:
    BlockHighlighter(LineboxEdit *editor, Dictionary *dict, QTextDocument *parent=nullptr);
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
    QString myProofedText;
    BlockHighlighter *myHighlighter;
    Dictionary *myDict;
    TextDistance *myDist;
    int myCurrentLine;
    void handleFrac();
public:
    LineboxEdit(Dictionary *dict, QWidget *parent=nullptr);
    ~LineboxEdit() {
        delete myDist;
    }

    void readFile(const QString& fileName);
    void writeFile(const QString& fileName);
    QChar currentChar() const;
    TextDistance *dist() const { return myDist; }
signals:
    void lineChanged(const QRect&);

private slots:
    void onCursorPosition();

public slots:
    void updateDist();
};

#endif // LINEBOXEDIT_H
