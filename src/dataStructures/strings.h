#ifndef STRINGS_H
#define STRINGS_H

#include "QVector"

class Strings
{
public:
    Strings();
    Strings(QVector< QVector<QByteArray> > stringsData);
    void setStringsData(QVector< QVector<QByteArray> > stringsData);
    int getMatrixLen();
    QByteArray getStringsOffsets();
    QByteArray getStrings();
private:
    QVector< QVector<QByteArray> > stringsMatrix;
    int matrixLen;
};

#endif // STRINGS_H
