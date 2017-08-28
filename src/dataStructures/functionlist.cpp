#include "functionlist.h"
#include "QDebug"

FunctionList::FunctionList()
{
    head = NULL;
    tail = NULL;
    length = 0;
    errorMsg = "";
}

void FunctionList::insert(QString name, QString address, QString section, QString fileOffset, QVector< QVector<QByteArray> > contents){
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

// Deletes all nodes in the list
void FunctionList::nukeList(){
    if (!isEmpty()){
        FunctionNode *current = head;
        FunctionNode *prev = head;

        while(current->getNext() != NULL){
            current = current->getNext();
            delete prev;
            prev = current;
        }
        delete current;

        head = NULL;
        tail = NULL;
        length = 0;
        errorMsg = "";

    }
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
    QVector< QVector<QByteArray> > empty;
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
    QVector< QVector<QByteArray> > empty;
    Function f("", "", "", "", empty);
    return f;
}

int FunctionList::getFunctionIndex(QString functionName){
    if (!isEmpty()){
        int counter = 0;
        if (head->getFunction().getName() == functionName)
            return counter;

        FunctionNode *p = head;
        while (p->getNext() != NULL){
            p = p->getNext();
            counter++;
            if (p->getFunction().getName() == functionName)
                return counter;
        }
    }
    // If not found return -1
    return -1;
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

// Get the location of a target address
QVector<int> FunctionList::getAddressLocation(QString targetAddress){
    QVector<int> location(2);

    // Search each function
    for (int functionIndex = 0; functionIndex < length; functionIndex++){
        Function function = getFunction(functionIndex);

        // Check if matrix is empty
        if (function.getMatrixLen() > 0){
            // Binary search
            int upperLimit = function.getMatrixLen() - 1;
            int lowerLimit = 0;
            int currentIndex = upperLimit / 2;

            while (lowerLimit != upperLimit && currentIndex != 0){
                QString currentAddress = function.getAddressAt(currentIndex);

                if (currentAddress == targetAddress){
                    location[0] = functionIndex;
                    location[1] = currentIndex;
                    return location;
                } else if (targetAddress < currentAddress){
                    upperLimit = currentIndex - 1;
                    currentIndex = (upperLimit + lowerLimit) / 2;
                } else{
                    lowerLimit = currentIndex + 1;
                    currentIndex = (upperLimit + lowerLimit) / 2;
                }

            }
            // Final checks
            if (function.getAddressAt(currentIndex) == targetAddress){
                location[0] = functionIndex;
                location[1] = currentIndex;
                return location;
            } else if (function.getMatrixLen() > 1 && function.getAddressAt(1) == targetAddress){
                location[0] = functionIndex;
                location[1] = 1;
                return location;
            }
        }


    }

    // If not found return [-1,-1]
    location[0] = -1;
    location[1] = -1;

    return location;
}

// Find all calls to a target function and return list of each calling function and address of call
QVector< QVector<QString> > FunctionList::findCallsToFunction(QString targetFunction){
    QVector< QVector<QString> > results;
    targetFunction = "<" + targetFunction + ">";

    // Search each function
    for (int functionIndex = 0; functionIndex < length; functionIndex++){
        Function function = getFunction(functionIndex);

        // Check if matrix is empty
        if (function.getMatrixLen() > 0){
            // Search function
            int matrixLen = function.getMatrixLen();
            for (int i = 0; i < matrixLen; i++){
                QVector<QByteArray> line = function.getLine(i);
                // Check for call to target function
                if (line[2] == "call" && QString::fromLocal8Bit(line[3]).contains(targetFunction)){
                    QVector<QString> result(2);
                    result[0] = function.getName();
                    result[1] = line[0];
                    results.append(result);
                }
            }

        }
    }

    return results;
}

// Find all calls to specific address
QVector< QVector<QString> > FunctionList::findCallsToAddress(QString targetAddress){
    QVector< QVector<QString> > results;

    // Search each function
    for (int functionIndex = 0; functionIndex < length; functionIndex++){
        Function function = getFunction(functionIndex);

        // Check if matrix is empty
        if (function.getMatrixLen() > 0){
            // Search function
            int matrixLen = function.getMatrixLen();
            for (int i = 0; i < matrixLen; i++){
                QVector<QByteArray> line = function.getLine(i);
                // Check for call to target function
                if (QString::fromLocal8Bit(line[3]).contains(targetAddress)){
                    QVector<QString> result(2);
                    result[0] = function.getName();
                    result[1] = line[0];
                    results.append(result);
                }
            }

        }
    }

    return results;
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


