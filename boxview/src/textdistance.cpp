#include <QtDebug>
#include "textdistance.h"

TextDistance::TextDistance(const QString &yString):
    myXstring(QString()), myYstring(yString)
{
    if (yString.isEmpty()) return;
    // row 0
    myYsize = yString.size()+1;
    myEntries.resize(myYsize*myYsize*2);
    myEntries[0] = Entry(0, 0);
    for (int y = 1; y < myYsize; y++) {
        myEntries[idx(0,y)] = Entry(idx(0, y), y);
    }
    for (int x = 0; x < 2*myYsize; x++) {
        myEntries[idx(x,0)] = Entry(idx(x, 0), x);
    }
    qDebug()<<"distance constructed";
}

int TextDistance::distance(const QString &xString, int startX) {
    qDebug()<<"distance";
    if (myYstring.isEmpty()) return 0;
    myXstring = xString;
    if (myXstring.size() >= 2*myYsize) {
        myXstring = myXstring.left(myYsize*2-1);
    }
    //appendMatrix(xString.size()+1);
    qDebug()<<"compare "<<myXstring.size()<<" "<<myYstring.size();
    return levenshtein(startX+1);
}

void TextDistance::appendMatrix(int newX) {
    /*
    if (newX*myYsize < myEntries.size()) return;
    myEntries.resize(newX*myYsize);
    for (int x = myEntries.size()/myYsize; x < newX; x++) {
        myEntries[idx(x,0)] = Entry(idx(x, 0), x);
        for (int y = 1; y < myYstring.size()+1; y++) {
            myEntries[idx(x, y)] = Entry(idx(x, y), 0);
        }
    }
    qDebug()<<"size "<<myEntries.size()<<"("<<(newX*myYsize)<<")";
    */
}

int TextDistance::levenshtein(int startX) {
    for (int x = startX; x < myXstring.size()+1; x++) {
        int pos = idx(x, 0);
        for (int y = 1; y < myYstring.size()+1; y++) {
            pos++;
            int prevPos = pos-myYsize-1; // x-1, y-1
            if (myEntries[pos-myYsize].dist < myEntries[prevPos].dist) { // x-1, y
                prevPos = pos-myYsize;
            }
            if (myEntries[pos-1].dist < myEntries[prevPos].dist) { // x, y-1
                prevPos = pos-1;
            }
            bool equal = myXstring[x-1] == myYstring[y-1];
            myEntries[pos].pos = pos;
            myEntries[pos].dist = myEntries[prevPos].dist + (equal? 0 : 1);
            myEntries[pos].equal = equal;
            myEntries[pos].prevPos = prevPos;
        }
    }
    int x = myXstring.size();
    int y = myYstring.size();
    int pos = idx(x, y);
    qDebug()<<"distance"<<myEntries[pos].dist;
    qDebug()<<"prev "<<myEntries[pos].prevPos;
    restorePath(myXstring.size(), myYstring.size());
    return myEntries[pos].dist;
}

void TextDistance::restorePath(int x, int y) {
    qDebug()<<"restorePath";
    myPath.clear();
    int pos = idx(x, y);
    while (pos % myYsize > 0 && pos > myYsize) { // not row 0/column 0
        myPath.prepend(myEntries[pos]);
        qDebug()<<myPath.size()<<" "<<myPath[0].pos;
        int prevPos = myEntries[pos].prevPos;
        pos = prevPos;
        //qDebug()<<pos/myYsize<<" "<<pos%myYsize;
    }
    qDebug()<<"path "<<myPath.size()<<" ";
    if (myPath.size() > 0) qDebug()<<myPath[0].pos;
}

bool TextDistance::isCharEqual(int x) const {
    if (myYstring.isEmpty()) return true;
    //qDebug()<<"isEqual "<<x;
    for (const Entry& entry: myPath) {
        int entryX = entry.pos/myYsize;
        if (entryX == x+1 && entry.equal) {
            return true;
        }
    }
    return false;
}

QString TextDistance::xPath() const {
    qDebug()<<"xPath";
    QString res;
    for (const Entry& entry: myPath) {
        int x = entry.pos/myYsize;
        //qDebug()<<x<<entry.pos%myYsize;
        res.append(myXstring[x-1]);
    }
    return res;
}
