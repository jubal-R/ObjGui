#include "strings.h"

Strings::Strings(){
    matrixLen = 0;
}

Strings::Strings(QVector<QVector<QString> > stringsData){
    stringsMatrix = stringsData;
    matrixLen = stringsMatrix.length();
}

void Strings::setStringsData(QVector<QVector<QString> > stringsData){
    stringsMatrix = stringsData;
    matrixLen = stringsMatrix.length();
}

QString Strings::getStrings(){
    QString strings;
    for (int i = 0; i < matrixLen; i++){
        QString str = stringsMatrix[i][1];
        str.replace('\n', "\\n");
        strings.append(str + "\n");
    }
    return strings;
}

QString Strings::getStringsAddresses(){
    QString offsets;
    for (int i = 0; i < matrixLen; i++){
        offsets.append(stringsMatrix[i][0] + "\n");
    }
    return offsets;
}

int Strings::getMatrixLen(){
    return matrixLen;
}
