#include "functionlist.h"
#include "iostream"

FunctionList::FunctionList()
{
    head = NULL;
    tail = NULL;
    length = 0;
    errorMsg = "";
}

void FunctionList::insert(QString name, QString address, QString section, QString fileOffset, QVector< QVector<QString> > contents){
    FunctionNode *newNode = new FunctionNode(name, address, section, fileOffset, contents);

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
    QVector< QVector<QString> > empty;
    Function f("", "", "", "", empty);
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
    QVector< QVector<QString> > empty;
    Function f("", "", "", "", empty);
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


void FunctionList::setErrorMsg(QString msg){
    errorMsg = msg;
}

QString FunctionList::getErrorMsg(){
    return errorMsg;
}

// Checks if list was created successfully(no errors)
bool FunctionList::successfullyCreated(){
    if (errorMsg == "")
        return true;
    else
        return false;
}
