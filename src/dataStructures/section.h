#ifndef SECTION_H
#define SECTION_H

#include "QString"
#include "QVector"

class Section
{
public:
    Section();
    Section(QString section, QVector< QVector<QString> > contents);
    QString getSectionName();
    int getNumLines();
    QVector<QString> getLine(int line);
    QString getHexString();
    QString getAddressString();
    QString getAsciiString();

private:
    QString sectionName;
    QVector< QVector<QString> > sectionMatrix;
    int matrixLen;
};

#endif // SECTION_H
