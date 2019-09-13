#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QtDebug>

#include "frakturhandler.h"
#include "lineboxedit.h"
#include "dictionary.h"
#include "textclassifier.h"

BlockHighlighter::BlockHighlighter(Dictionary *dict, QTextDocument *parent):
    QSyntaxHighlighter(parent),
    myDict(dict)
{
    myGapExp = QRegularExpression("^ |[.!?,;:] $");
    myWordFormat.setForeground(Qt::red);
    myLongSFormat.setForeground(Qt::magenta);
    myGapFormat.setBackground(Qt::yellow);
    mySFormat.setForeground(Qt::blue);
}

void BlockHighlighter::highlightBlock(const QString &text) {
    TextClassifier classifier;
    QList<Block> blocks(classifier.classify(text));
    qDebug()<<"Highlight blocks"<<blocks.size();
    for (const Block &block: blocks) {
        if (block.type() == TextClassifier::C_LETTER) {
            if (!myDict->contains(block.text())) {
                if (myDict->containsLongS(block.text())) {
                    setFormat(block.start(), block.len(), myLongSFormat);
                } else {
                    setFormat(block.start(), block.len(), myWordFormat);
                }
            }
        } else if (block.type() == TextClassifier::C_OTHER) {
            if (isUnusualGap(block.text())) {
                setFormat(block.start(), block.len(), myGapFormat);
            }
        }
    }
    for (int i = 0; i < text.size()-1; i++) {
        if (text[i] == 's' && text[i+1].isLetter()) {
            setFormat(i, 1, mySFormat);
        }
    }
}

bool BlockHighlighter::isUnusualGap(const QString &text) const {
    return !myGapExp.match(text).hasMatch();
}

LineboxEdit::LineboxEdit(Dictionary *dict, QWidget *parent):
    QTextEdit(parent), myDict(dict), myCurrentLine(-1) {
    setFont(QFont("DPCustomMono2", 12));
    myHighlighter = new BlockHighlighter(myDict, this->document());
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPosition()));
}

void LineboxEdit::readFile(const QString &baseName) {
    bool firstPass = false;
    QString fileName(baseName);
    if (QFileInfo::exists(baseName+".box")) {
         fileName += ".box";
    } else {
         fileName += ".tif.box";
         if(!QFileInfo::exists(fileName)) return;
         firstPass = true;
    }
    myFileName = baseName+".box";
    QFile fi(fileName);
    fi.open(QFile::ReadOnly|QFile::Text);
    QTextStream in(&fi);
    myLines.clear();
    myBoxes.clear();
    QString textline;
    while (!in.atEnd()) {
        QString line = in.readLine();
        int i = line.indexOf(" ", 1);
        QString c = line.left(i);
        qDebug()<<line<<c;
        if (c == '\t') {
            QString box = line.mid(i+1);
            QStringList parts = box.split(" ");
            QRect bbox(parts[0].toInt(), parts[1].toInt(), parts[2].toInt(), parts[3].toInt());
            myBoxes.append(bbox);
            myLines.append(textline);
            textline.clear();
        } else {
            textline.append(c);
        }
    }
    fi.close();
    qDebug()<<myLines.size();
    if (firstPass) handleFrac();
    qDebug()<<myLines.join("\n");
    setPlainText(myLines.join("\n"));
    myCurrentLine = -1;
    textCursor().setPosition(0);
}

void LineboxEdit::writeFile(const QString &fileName) {
    QStringList lines = toPlainText().split("\n");
    QFile fo(fileName);
    if (!fo.open(QFile::WriteOnly|QFile::Text)) return;
    QTextStream out(&fo);
    out.setCodec("UTF-8");
    for (int i = 0; i < lines.size(); i++) {
        for (const QChar& c: lines[i]) {
            out<<c<<" "<<myBoxes[i].x()<<" "<<myBoxes[i].y()<<" "<<myBoxes[i].width()<<" "<<myBoxes[i].height()<<endl;
        }
        out<<"\t "<<myBoxes[i].x()<<" "<<myBoxes[i].y()<<" "<<myBoxes[i].width()<<" "<<myBoxes[i].height()<<endl;
    }
    fo.close();
}

void LineboxEdit::onCursorPosition() {
    if (textCursor().blockNumber() != myCurrentLine) {
        myCurrentLine = textCursor().blockNumber();
        qDebug()<<"new line "<<myCurrentLine<<myBoxes[myCurrentLine];
        emit lineChanged(myBoxes[myCurrentLine]);
    }
}

void LineboxEdit::handleFrac() {
    FrakturHandler handler;
    QStringList newLines;
    for (const QString& line: myLines) {
        QString newLine = handler.handle(line);
        newLines.append(newLine);
    }
    myLines = newLines;
}

QChar LineboxEdit::currentChar() const {
    return toPlainText().at(textCursor().position());
}
