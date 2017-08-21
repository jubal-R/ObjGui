#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H

#include "function.h"

class FunctionNode
{
public:
    FunctionNode(QString name, QString address, QString section, QString fileOffset, QVector< QVector<QString> > contents);
    void setNext(FunctionNode *n);
    FunctionNode* getNext();
    Function getFunction();
private:
    FunctionNode *next;
    Function function;

};

#endif // FUNCTIONNODE_H
