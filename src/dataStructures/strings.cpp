#include "strings.h"

Strings::Strings(){
    matrixLen = 0;
}

Strings::Strings(QVector<QVector<QByteArray> > stringsData){
    stringsMatrix = stringsData;
    matrixLen = stringsMatrix.length();
}

void Strings::setStringsData(QVector<QVector<QByteArray> > stringsData){
    stringsMatrix = stringsData;
    matrixLen = stringsMatrix.length();
}

QByteArray Strings::getStrings(){
    QByteArray strings;
    for (int i = 0; i < matrixLen; i++){
        strings.append(stringsMatrix[i][1] + "\n");
    }
    return strings;
}

QByteArray Strings::getStringsOffsets(){
    QByteArray offsets;
    for (int i = 0; i < matrixLen; i++){
        offsets.append(stringsMatrix[i][0] + "\n");
    }
    return offsets;
}

int Strings::getMatrixLen(){
    return matrixLen;
}
