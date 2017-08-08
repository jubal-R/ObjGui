#include "function.h"

Function::Function(){
    name = "";
    address = "";
    contents = "";
}

Function::Function(QString functionName, QString addr, QString functionContents)
{
    name = functionName;
    address = addr;
    contents = functionContents;
}

//Get Values

QString Function::getName(){
    return name;
}

QString Function::getAddress(){
    return address;
}

QString Function::getContents(){
    return contents;
}
