#ifndef PATTERN_H
#define PATTERN_H

#include <QSet>
#include <QString>

class Pattern {
private:
    QSet<QString> myWords;
    QSet<QString> myGaps;
public:
    Pattern(const QSet<QString>& words, const QSet<QString>& gaps):
        myWords(words), myGaps(gaps)
    {}
    bool containsWord(const QString& word) const {
        return myWords.contains(word);
    }
    bool containsGap(const QString& gap) const {
        return myGaps.contains(gap);
    }
    int wordSize() const {
        return myWords.size();
    }
    int gapSize() const {
        return myGaps.size();
    }
    // debug
    void showGaps() const;
};

#endif // PATTERN_H
