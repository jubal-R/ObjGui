#include "functionnode.h"

//Function function;

FunctionNode::FunctionNode(QString name, QString address, QString section, QString fileOffset, QVector< QVector<QByteArray> > contents)
    : function(name, address, section, fileOffset, contents)
{
    next = NULL;

}

// Set Next Node
void FunctionNode::setNext(FunctionNode *n){
    next = n;
}

// Get Next Node
FunctionNode* FunctionNode::getNext(){
    return next;
}

// Get Function Object

Function FunctionNode::getFunction(){
    return function;
}
