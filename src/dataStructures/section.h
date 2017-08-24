#ifndef SECTION_H
#define SECTION_H

#include "QString"
#include "QVector"

class Section
{
public:
    Section();
    Section(QString section, QVector< QVector<QByteArray> > contents);
    QString getSectionName();
    int getNumLines();
    QVector<QByteArray> getLine(int line);
    QByteArray getHexString();
    QByteArray getAddressString();
    QByteArray getAsciiString();

private:
    QString sectionName;
    QVector< QVector<QByteArray> > sectionMatrix;
    int matrixLen;
};

#endif // SECTION_H
