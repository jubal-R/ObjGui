#include "section.h"

/*
 * Section contents stored in a matrix where each row is a line of output.
 * Colomns contain {address, hex, ascii}
 * Example:
 *      [000000]    [0b 0a 3c]    [1..hello ]
 *      [000010]    [0b 0a 3c]    [world....]
 *      [000020]    [12 c9 5d]    [..c.h.cv.]
 *      ...
 */

Section::Section()
{

}

Section::Section(QString section, QVector< QVector<QString> > contents){
    sectionName = section;
    sectionMatrix = contents;
    matrixLen = sectionMatrix.length();
}

QString Section::getSectionName(){
    return sectionName;
}

int Section::getNumLines(){
    return matrixLen;
}

// Return a line(row) from the matrix
QVector<QString> Section::getLine(int line){
    return sectionMatrix.at(line);
}

// Get a string of all hex values seperated by line breaks
QString Section::getHexString(){
    QString hexStr = "";
    for (int i = 0; i < matrixLen; i++){
        hexStr.append(sectionMatrix.at(i)[1] + "\n");
    }

    return hexStr;
}

// Get a string of all address values seperated by line breaks
QString Section::getAddressString(){
    QString addrStr = "";
    for (int i = 0; i < matrixLen; i++){
        addrStr.append(sectionMatrix.at(i)[0] + "\n");
    }

    return addrStr;
}

// Get a string of all ascii values seperated by line breaks
QString Section::getAsciiString(){
    QString asciiStr = "";
    for (int i = 0; i < matrixLen; i++){
        asciiStr.append(sectionMatrix.at(i)[2] + "\n");
    }

    return asciiStr;
}
