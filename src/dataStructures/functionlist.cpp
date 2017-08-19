#include "functionlist.h"
#include "iostream"

FunctionList::FunctionList()
{
    head = NULL;
    tail = NULL;
    length = 0;
}

void FunctionList::insert(QString name, QString address, QString contents, QString section, QString fileOffset){
    FunctionNode *newNode = new FunctionNode(name, address, contents, section, fileOffset);

    if (isEmpty()){
        head = newNode;
        tail = head;
    } else {
        tail->setNext(newNode);
        tail = tail->getNext();
    }
    length++;
}

bool FunctionList::isEmpty(){
    if (head == NULL)
        return true;
    else
        return false;
}

int FunctionList::getLength(){
    return length;
}

bool FunctionList::containsFunction(QString name){
    if (!isEmpty()){
        if (head->getFunction().getName() == name)
            return true;

        FunctionNode *p = head;
        while (p->getNext() != NULL){
            p = p->getNext();
            if (p->getFunction().getName() == name)
                return true;
        }
    }
    return false;
}

Function FunctionList::getFunction(QString name){
    if (!isEmpty()){
        if (head->getFunction().getName() == name)
            return head->getFunction();

        FunctionNode *p = head;
        while (p->getNext() != NULL){
            p = p->getNext();
            if (p->getFunction().getName() == name)
                return p->getFunction();
        }
    }
    // If not found return empty function
    Function f("", "", "", "", "");
    return f;
}

Function FunctionList::getFunction(int index){
    if (!isEmpty() && index < length){
        FunctionNode *p = head;
        for (int i = 0; i < index; i++){
            p = p->getNext();
        }
        return p->getFunction();
    }
    // If not found return empty function
    Function f("", "", "", "", "");
    return f;
}

QStringList FunctionList::getFunctionNames(){
    QStringList functionNames;
    if (!isEmpty()){
        functionNames.append(head->getFunction().getName());
        FunctionNode *p = head;
        while (p->getNext() != NULL){
            p = p->getNext();
            functionNames.append(p->getFunction().getName());
        }
    }

    return functionNames;
}


