#ifndef FUNCTION_H
#define FUNCTION_H

#include "QString"

class Function
{
public:
    Function();
    Function(QString functionName, QString addr, QString functionContents, QString section, QString offset);
    QString getName();
    QString getAddress();
    QString getContents();
    QString getSection();
    QString getFileOffset();
private:
    QString name;
    QString address;
    QString contents;
    QString section;
    QString fileOffset;

};

#endif // FUNCTION_H
