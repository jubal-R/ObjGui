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

int Strings::getIndexByAddress(QString targetAddress){
    if (!stringsMatrix.isEmpty()){
        // Binary search
        int upperLimit = matrixLen - 1;
        int lowerLimit = 0;
        int currentIndex = upperLimit / 2;

        while (lowerLimit != upperLimit && currentIndex != 0){
            QString currentAddress = stringsMatrix[currentIndex][0];

            if (currentAddress == targetAddress){
                return currentIndex;
            } else if (targetAddress < currentAddress){
                upperLimit = currentIndex - 1;
                currentIndex = (upperLimit + lowerLimit) / 2;
            } else{
                lowerLimit = currentIndex + 1;
                currentIndex = (upperLimit + lowerLimit) / 2;
            }

        }
        // Final checks
        if (stringsMatrix[currentIndex][0] == targetAddress){
            return currentIndex;
        } else if (matrixLen > 1 && stringsMatrix[1][0] == targetAddress){
            return 1;
        }
    }
    // If not found return -1
    return -1;
}

QString Strings::getAddressAt(int index){
    if(index >= 0 && index < matrixLen)
        return stringsMatrix[index][0];
    else
        return "";
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
