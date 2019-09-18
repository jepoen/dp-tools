#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QtDebug>
#include <cassert>

#include "frakturhandler.h"
#include "lineboxedit.h"
#include "dictionary.h"
#include "textclassifier.h"
#include "textdistance.h"

BlockHighlighter::BlockHighlighter(LineboxEdit *editor, Dictionary *dict, QTextDocument *parent):
    QSyntaxHighlighter(parent),
    myEditor(editor),
    myDict(dict)
{
    myGapExp = QRegularExpression("^ |[.!?,;:] $");
    myWordFormat.setForeground(Qt::blue);
    myLongSFormat.setForeground(Qt::magenta);
    myGapFormat.setBackground(Qt::gray);
    myDistanceFormat.setBackground(Qt::yellow);
}

void BlockHighlighter::highlightBlock(const QString &text) {
    TextClassifier classifier;
    QList<Block> blocks(classifier.classify(text));
    //qDebug()<<"Highlight blocks"<<blocks.size();
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
    int startPos = currentBlock().position();
    QList<int> correspond = myEditor->dist()->correspondingValues();
    for (int i = 0; i < text.size(); i++) {
        if (startPos + i >= correspond.size()) break;
        if (correspond[startPos+i] < 0) {
            setFormat(i, 1, myDistanceFormat);
        }
    }
}

bool BlockHighlighter::isUnusualGap(const QString &text) const {
    return !myGapExp.match(text).hasMatch();
}

LineboxEdit::LineboxEdit(Dictionary *dict, QWidget *parent):
    QTextEdit(parent), myDict(dict), myDist(nullptr), myCurrentLine(-1) {
    setFont(QFont("DPCustomMono2", 12));
    myHighlighter = new BlockHighlighter(this, myDict, this->document());
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPosition()));
    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

void LineboxEdit::readFile(const QString &baseName) {
    bool firstPass = false;
    delete myDist;
    myDist = nullptr;
    //myCorrespond.clear();
    myChanged = false;
    QString fileName(baseName);
    if (QFileInfo::exists(baseName+".box")) {
         fileName += ".box";
    } else {
         fileName += "-raw.box";
         if(!QFileInfo::exists(fileName)) return;
         firstPass = true;
    }
    // proofed text
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
        //qDebug()<<line<<c;
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
    if (QFileInfo::exists(baseName+".txt")) {
        QFile fi(baseName+".txt");
        fi.open(QFile::ReadOnly|QFile::Text);
        QTextStream in(&fi);
        in.setCodec("ISO-8859-15");
        QStringList proofedLines;
        while (!in.atEnd()) {
            QString line = in.readLine();
            line = handleProofedLine(line);
            proofedLines.append(line);
        }
        fi.close();
        qDebug()<<"proofed"<<proofedLines.join(" ");
        myProofedText = proofedLines.join("\n");
        myDist = new TextDistance(proofedLines.join(" "));
    }
    qDebug()<<myLines.size();
    if (firstPass) handleFrac();
    qDebug()<<myLines.join("\n");
    setPlainText(myLines.join("\n"));
    myCurrentLine = -1;
    updateDist();
    setFocus();
}

void LineboxEdit::writeFile(const QString &fileName) {
    QStringList lines = toPlainText().split("\n");
    if (lines.size() != myBoxes.size()) {
        QMessageBox::warning(this, tr("Wrong line number"),
                             tr("Text line number differs from box number.\nCannot save file."));
        return;
    }
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
    myChanged = false;
}

void LineboxEdit::onCursorPosition() {
    if (textCursor().blockNumber() != myCurrentLine) {
        myCurrentLine = textCursor().blockNumber();
        if (myCurrentLine < myBoxes.size()) {
            qDebug()<<"new line "<<myCurrentLine<<myBoxes[myCurrentLine];
            emit lineChanged(myBoxes[myCurrentLine]);
        } else {
            // TODO empty page
        }
    }
}

void LineboxEdit::onTextChanged() {
    myChanged = true;
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

QString LineboxEdit::handleProofedLine(QString line) {
    line.replace("--", "—");
    line.replace("<", "‹");
    line.replace(">", "›");
    return line;
}

QChar LineboxEdit::currentChar() const {
    return document()->characterAt(textCursor().position());
}

void LineboxEdit::updateDist() {
    qDebug()<<"update dist";
    //QProgressDialog msg("Compute distance", "Distance", 0, 100, this);
    int dist = myDist->distance(document()->toPlainText());
    QList<int> correspond = myDist->correspondingValues();
    qDebug()<<dist<<correspond;
    myHighlighter->rehighlight();
}

void LineboxEdit::replaceFromProofed() {
    qDebug()<<"replaced from proofed text";
    QString text = document()->toPlainText();
    if (text.size() == 0) return;
    myDist->distance(text);
    QList<int> correspond = myDist->correspondingValues();
    assert(text.size() == correspond.size());
    for (int i = 1; i < text.size()-1; i++) {
        if (text[i] == '\n') continue;
        if (correspond[i] < 0) {
            qDebug()<<"replace "<<i<<" "<<text[i-1]<<text[i]<<text[i+1];
            if (correspond[i-1] >= 0 && correspond[i+1] >= 0) {
                qDebug()<<" corresponds"<<correspond[i-1]<<" "
                        <<correspond[i+1];
                // debug
                QString repl;
                for (int k = correspond[i-1]; k <= correspond[i+1]; k++) {
                    repl += myProofedText[k];
                }
                qDebug()<<" corresponds"<<repl;
                if (correspond[i+1]-correspond[i-1] == 1) { // superfluous letter
                    text[i] = QChar(0x3fff);
                } else {
                    text[i] = myProofedText[correspond[i-1]+1];
                }
            }
        }
    }
    int oldSize = text.size();
    text = text.replace(QString("\u3fff"), "");
    if (oldSize != text.size()) {
        myDist->distance(text);
    }
    document()->setPlainText(text);
}

void LineboxEdit::deleteLine() {
    QTextCursor cursor = textCursor();
    int line = cursor.blockNumber();
    cursor.movePosition(QTextCursor::StartOfBlock);
    if (line < myBoxes.size()-1) {
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        myBoxes.removeAt(line);
        emit lineChanged(myBoxes[line]);
    } else if (line == myBoxes.size()-1) {
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        myBoxes.removeAt(line);
        line--;
        cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, line);
        emit lineChanged(myBoxes[line]);
    }
    setTextCursor(cursor);
}
