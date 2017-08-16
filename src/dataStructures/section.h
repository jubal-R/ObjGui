#ifndef SECTION_H
#define SECTION_H

#include "QString"
#include "QStringList"

class Section
{
public:
    Section();
    Section(QString section, QStringList addresses, QStringList hex, QStringList ascii);
    QString getSectionName();
    QStringList getAddressList();
    QStringList getHexList();
    QStringList getAsciiList();

private:
    QString sectionName;
    QStringList addressList;
    QStringList hexList;
    QStringList asciiList;
};

#endif // SECTION_H
