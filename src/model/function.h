#ifndef FUNCTION_H
#define FUNCTION_H

#include "QString"
#include "QVector"

class Function
{
public:
    Function();
    Function(QString functionName, QString addr, QString section, QString offset, QVector< std::array<QByteArray, 5> > contents);
    int getMatrixLen();
    bool isEmpty();
    void setXrefData(int index, QString xrefData);
    QString getName();
    QString getAddress();
    QString getSection();
    QString getFileOffset();
    QString getAddressAt(int index);
    std::array<QByteArray, 5> getLine(int line);
    QByteArray getContents();
private:
    QString name;
    QString address;
    QString section;
    QString fileOffset;
    QVector< std::array<QByteArray, 5> > functionMatrix;
    int matrixLen;

};

#endif // FUNCTION_H
