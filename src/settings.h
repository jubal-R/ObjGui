#ifndef SETTINGS_H
#define SETTINGS_H

#include "QString"

class Settings
{
public:
    Settings();
    void saveSettings();
    void setWindowWidth(int width);
    void setWindowHeight(int height);
    void setSyntax(QString syntax);
    int getWindowWidth();
    int getWindowHeight();
    QString getSyntax();

private:
    int windowWidth;
    int windowHeight;
    QString syntax;
    QString disassemblyFlag;

};

#endif // SETTINGS_H
