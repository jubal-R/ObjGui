#ifndef FUNCTION_H
#define FUNCTION_H

#include "QString"
#include "QVector"

class Function
{
public:
    Function();
    Function(QString functionName, QString addr, QString section, QString offset, QVector< QVector<QByteArray> > contents);
    int getMatrixLen();
    bool isEmpty();
    QString getName();
    QString getAddress();
    QString getSection();
    QString getFileOffset();
    QString getAddressAt(int index);
    QVector<QByteArray> getLine(int line);
    QByteArray getContents();
private:
    QString name;
    QString address;
    QString section;
    QString fileOffset;
    QVector< QVector<QByteArray> > functionMatrix;
    int matrixLen;

};

#endif // FUNCTION_H
