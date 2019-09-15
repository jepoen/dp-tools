#ifndef TEXTDISTANCE_H
#define TEXTDISTANCE_H

#include <QList>
#include <QString>

/***
 * Levenshtein distance between ocr and proofed page
 * x - ocr
 * y - proofed
 */
class TextDistance {
public:
    class Entry {
    public:
        int pos;
        int dist;
        int prevPos;
        bool equal;
        Entry(int p=0, int d=0): pos(p), dist(d), prevPos(-1), equal(false) {}
    };
private:
    QString myXstring;
    QString myYstring;
    int myYsize;
    QVector<Entry> myEntries;
    QList<Entry> myPath;
    int myDistance;
    int idx(int x, int y) const { return x*myYsize+y; }
    void appendMatrix(int newX);
    int levenshtein(int startX = 0);
    void restorePath(int x, int y);
public:
    TextDistance(const QString& yString = QString());
    int distance(const QString& xString, int startX = 0);
    bool isCharEqual(int x) const;
    QString xPath() const;
};

#endif // TEXTDISTANCE_H
