#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings {
private:
    double myPixelScale;
    QString myFontName;
    int myFontSize;

public:
    Settings(double pixelSize=1.0, const QString& fontName="DPSansMono", int fontSize=14);
    double pixelScale() const { return myPixelScale; }
    QString fontName() const { return myFontName; }
    int fontSize() const { return myFontSize; }
    void setPixelScale(double pixelScale) { myPixelScale = pixelScale; }
    void setFontName(const QString& fontName) { myFontName = fontName; }
    void setFontSize(int fontSize) { myFontSize = fontSize; }
};

#endif // SETTINGS_H
