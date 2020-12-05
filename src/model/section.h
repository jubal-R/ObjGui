#ifndef SECTION_H
#define SECTION_H

#include "QString"
#include "QVector"

class Section
{
public:
    Section();
    Section(QString section, QVector< std::array<QByteArray, 2> > contents);
    QString getSectionName();
    QString getAddressAt(int index);
    int getMatrixLen();
    std::array<QByteArray, 2> getLine(int line);
    QByteArray getHexString();
    QByteArray getAddressString();

private:
    QString sectionName;
    QVector< std::array<QByteArray, 2> > sectionMatrix;
    int matrixLen;
};

#endif // SECTION_H
