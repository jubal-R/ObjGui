#ifndef FUNCTIONLIST_H
#define FUNCTIONLIST_H

#include "functionnode.h"
#include "function.h"
#include "QStringList"

class FunctionList
{
public:
    FunctionList();
    void insert(QString name, QString address, QString contents, QString section);
    int getLength();
    bool isEmpty();
    bool containsFunction(QString name);
    Function getFunction(QString name);
    Function getFunction(int index);
    QStringList getFunctionNames();
private:
    FunctionNode *head;
    FunctionNode *tail;
    int length;
};

#endif // FUNCTIONLIST_H
