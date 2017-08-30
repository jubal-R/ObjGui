#include "function.h"

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

Function::Function(QString functionName, QString addr, QString sect, QString offset, QVector< QVector<QByteArray> > contents)
{
    name = functionName;
    address = addr;
    section = sect;
    fileOffset = offset;
    functionMatrix = contents;
    matrixLen = functionMatrix.length();
}

// Return a line(row) from the matrix
QVector<QByteArray> Function::getLine(int line){
    return functionMatrix.at(line);
}

QByteArray Function::getContents(){
    QByteArray contents;
    for (int i = 0; i < matrixLen; i++){
        QVector<QByteArray> line = getLine(i);
        contents.append(line[0] + "  ");
        contents.append(line[1] + " ");
        contents.append(line[2] + "  ");
        contents.append(line[3] + "\n");
    }

    return contents;
}

QString Function::getAddressAt(int index){
    if (index >= 0 && index < matrixLen)
        return functionMatrix.at(index)[0];
    else
        return "";
}

int Function::getMatrixLen(){
    return matrixLen;
}

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
