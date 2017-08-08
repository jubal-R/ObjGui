#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H

#include "function.h"
#include "QString"

class FunctionNode
{
public:
    FunctionNode(QString name, QString address, QString contents);
    void setNext(FunctionNode *n);
    FunctionNode* getNext();
    Function getFunction();
private:
    FunctionNode *next;
    Function function;

};

#endif // FUNCTIONNODE_H
