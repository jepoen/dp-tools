#include <QFile>
#include <QTextStream>
#include "dictionary.h"

Dictionary::Dictionary(const QString& fileName): myFileName(fileName) {
    readFile(myFileName);
}

void Dictionary::readFile(const QString &fileName) {
    QFile fi(fileName);
    if (fi.open(QFile::ReadOnly|QFile::Text)) {
        QTextStream in(&fi);
        in.setCodec("UTF-8");
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.trimmed().isEmpty()) {
                myDict.insert(line.trimmed(), true);
            }
        }
    }
}

bool Dictionary::containsLongS(const QString &word) const {
    QString w(word);
    w.replace("ſ", "s");
    return myDict.value(w, false);
}
