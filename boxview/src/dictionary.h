#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QMap>
#include <QString>

class Dictionary
{
private:
    QString myFileName;
    QMap<QString, bool> myDict;
public:
    Dictionary(const QString& fileName);
    void readFile(const QString& fileName);
    bool contains(const QString& word) const { return myDict.value(word, false); }
    bool containsLongS(const QString& word) const;
};

#endif // DICTIONARY_H
