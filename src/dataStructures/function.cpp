#include "function.h"
#include "QVector"

/*
 * Function contents stored in a matrix where each row is a line of output.
 * Colomns contain {address, hex, optcode, arguments}
 * Example:
 *      [000000]    [0b 0a 3c]    [push]    [rbp]
 *      [000010]    [0b 0a 3c]    [jmp]     [000050]
 *      [000020]    [12 c9 5d]    [call]    [00230b <printf@plt>]
 *      ...
 */

Function::Function(){

}

Function::Function(QString functionName, QString addr, QString sect, QString offset, QVector< QVector<QString> > contents)
{
    name = functionName;
    address = addr;
    section = sect;
    fileOffset = offset;
    functionMatrix = contents;
    matrixLen = functionMatrix.length();
}

// Return a line(row) from the matrix
QVector<QString> Function::getLine(int line){
    return functionMatrix.at(line);
}

QString Function::getContents(){
    QString contents = "";
    for (int i = 0; i < matrixLen; i++){
        QVector<QString> line = getLine(i);
        contents.append(line[0] + "    ");
        contents.append(line[1] + "    ");
        contents.append(line[2] + "    ");
        contents.append(line[3] + "\n");
    }

    return contents;
}

//Get Values

QString Function::getName(){
    return name;
}

QString Function::getAddress(){
    return address;
}

QString Function::getSection(){
    return section;
}

QString Function::getFileOffset(){
    return fileOffset;
}
