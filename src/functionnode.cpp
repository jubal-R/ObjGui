#include "functionnode.h"
#include "function.h"

//Function function;

FunctionNode::FunctionNode(QString name, QString address, QString contents)
    : function(name, address, contents)
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
