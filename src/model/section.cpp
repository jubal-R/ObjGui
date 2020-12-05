#include "section.h"

/*
 * Section contents stored in a matrix where each row is a line of output.
 * Colomns contain {address, hex}
 * Example:
 *      [000000]    [0b 0a 3c]
 *      [000010]    [0b 0a 3c]
 *      [000020]    [12 c9 5d]
 *      ...
 */

Section::Section()
{

}

Section::Section(QString section, QVector< std::array<QByteArray, 2> > contents){
    sectionName = section;
    sectionMatrix = contents;
    matrixLen = sectionMatrix.length();
}

// Return address at given index
QString Section::getAddressAt(int index){
    if (index >= 0 && index < matrixLen)
        return sectionMatrix.at(index)[0];
    else
        return QLatin1String("");
}

// Return the line(row) from the matrix at the given index/line number
std::array<QByteArray, 2> Section::getLine(int line){
    if (line > 0 && line < matrixLen)
        return sectionMatrix.at(line);
    else {
        return {};
    }
}

// Get a string of all hex values seperated by line breaks
QByteArray Section::getHexString(){
    QByteArray hexStr;
    for (int i = 0; i < matrixLen; i++){
        hexStr.append(sectionMatrix.at(i)[1] + "\n");
    }

    return hexStr;
}

// Get a string of all address values seperated by line breaks
QByteArray Section::getAddressString(){
    QByteArray addrStr = "";
    for (int i = 0; i < matrixLen; i++){
        addrStr.append(sectionMatrix.at(i)[0] + "\n");
    }

    return addrStr;
}

QString Section::getSectionName(){
    return sectionName;
}

int Section::getMatrixLen(){
    return matrixLen;
}
