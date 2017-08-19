#include "function.h"

Function::Function(){
    name = "";
    address = "";
    contents = "";
}

Function::Function(QString functionName, QString addr, QString functionContents, QString sect, QString offset)
{
    name = functionName;
    address = addr;
    contents = functionContents;
    section = sect;
    fileOffset = offset;
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

QString Function::getSection(){
    return section;
}

QString Function::getFileOffset(){
    return fileOffset;
}
