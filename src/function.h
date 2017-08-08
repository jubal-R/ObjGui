#ifndef FUNCTION_H
#define FUNCTION_H

#include "QString"

class Function
{
public:
    Function();
    Function(QString functionName, QString addr, QString functionContents);
    QString getName();
    QString getAddress();
    QString getContents();
private:
    QString name;
    QString address;
    QString contents;

};

#endif // FUNCTION_H
