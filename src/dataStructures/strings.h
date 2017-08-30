#ifndef STRINGS_H
#define STRINGS_H

#include "QVector"

class Strings
{
public:
    Strings();
    Strings(QVector< QVector<QString> > stringsData);
    void setStringsData(QVector< QVector<QString> > stringsData);
    int getMatrixLen();
    int getIndexByAddress(QString address);
    QString getStringsAddresses();
    QString getStrings();
private:
    QVector< QVector<QString> > stringsMatrix;
    int matrixLen;
};

#endif // STRINGS_H
