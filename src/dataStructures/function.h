#ifndef FUNCTION_H
#define FUNCTION_H

#include "QString"
#include "QVector"

class Function
{
public:
    Function();
    Function(QString functionName, QString addr, QString section, QString offset, QVector< QVector<QString> > contents);
    int getMatrixLen();
    QString getName();
    QString getAddress();
    QString getSection();
    QString getFileOffset();
    QString getAddressAt(int index);
    QVector<QString> getLine(int line);
    QString getContents();
private:
    QString name;
    QString address;
    QString section;
    QString fileOffset;
    QVector< QVector<QString> > functionMatrix;
    int matrixLen;

};

#endif // FUNCTION_H
