#include "function.h"

#include "QDebug"

/*
 * Function contents stored in a matrix where each row is a line of output.
 * Colomns contain {address, hex, optcode, arguments, xref data}
 * Example:
 *      [000010]    [0b 0a 3c]    [jmp]     [000050]                 []
 *      [000000]    [0b 0a 3c]    [lea]     [400981]                 ["HelloWorld"]
 *      [000020]    [12 c9 5d]    [call]    [00230b <printf@plt>]    []
 *      ...
 */

Function::Function(){
    matrixLen = 0;
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

void Function::setXrefData(int index, QString xrefData){
    functionMatrix[index][4] = "\"" + xrefData.toLatin1() + "\"";
}

// Return the line(row) from the matrix at given index/line number
QVector<QByteArray> Function::getLine(int line){
    if (line >= 0 && line < matrixLen)
        return functionMatrix.at(line);
    else {
        QVector<QByteArray> empty;
        return empty;
    }
}

// Return contents of function matrix formatted for display
QByteArray Function::getContents(){
    QByteArray contents;
    for (int i = 0; i < matrixLen; i++){
        QVector<QByteArray> line = getLine(i);
        contents.append(line[0] + "  ");
        contents.append(line[1] + " ");
        contents.append(line[2] + " ");
        contents.append(line[3] + "    ");
        contents.append(line[4] + "\n");
    }

    return contents;
}

// Retrun address at given index
QString Function::getAddressAt(int index){
    if (index >= 0 && index < matrixLen)
        return functionMatrix.at(index)[0];
    else
        return "";
}

bool Function::isEmpty(){
    if (matrixLen > 0)
        return true;
    else
        return false;
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
