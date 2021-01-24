#ifndef PATTERN_H
#define PATTERN_H

#include <QSet>
#include <QString>

class Pattern {
private:
    QSet<QString> myWords;
    QSet<QString> myGaps;
    QSet<QString> myEndGaps;
public:
    Pattern(const QSet<QString>& words, const QSet<QString>& gaps, QSet<QString>& endGaps):
        myWords(words), myGaps(gaps), myEndGaps(endGaps)
    {}
    bool containsWord(const QString& word) const {
        return myWords.contains(word);
    }
    bool containsGap(const QString& gap) const {
        return myGaps.contains(gap);
    }
    bool containsEndGap(const QString& gap) const {
        return myEndGaps.contains(gap);

    }
    int wordSize() const {
        return myWords.size();
    }
    int gapSize() const {
        return myGaps.size();
    }
    int endGapSiez() const {
        return myEndGaps.size();
    }
    // debug
    void showGaps() const;
};

#endif // PATTERN_H
