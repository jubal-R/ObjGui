#ifndef FUNCTION_H
#define FUNCTION_H

#include "QString"

class Function
{
public:
    Function();
    Function(QString functionName, QString addr, QString functionContents, QString section);
    QString getName();
    QString getAddress();
    QString getContents();
    QString getSection();
private:
    QString name;
    QString address;
    QString contents;
    QString section;

};

#endif // FUNCTION_H
