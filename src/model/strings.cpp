#include "strings.h"

#include "QDebug"

Strings::Strings(){
    matrixLen = 0;
}

Strings::Strings(QVector<QVector<QString> > stringsData){
    stringsMatrix = stringsData;
    matrixLen = stringsMatrix.length();
}

// Sets the strings matrix
void Strings::setStringsData(QVector<QVector<QString> > stringsData){
    stringsMatrix = stringsData;
    matrixLen = stringsMatrix.length();
}

// Returns the index of a target address
int Strings::getIndexByAddress(QString targetAddress){
    if (!stringsMatrix.isEmpty()){
        // Binary search
        int upperLimit = matrixLen - 1;
        int lowerLimit = 0;
        int currentIndex = upperLimit / 2;

        while (lowerLimit < upperLimit && currentIndex != 0){
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

// Returns address at given index
QString Strings::getAddressAt(int index){
    if(index >= 0 && index < matrixLen)
        return stringsMatrix[index][0];
    else
        return "";
}

QString Strings::getStringAt(int index){
    if(index >= 0 && index < matrixLen){
        QString str = stringsMatrix[index][1].replace('\n', "\\n");
        return str;
    } else {
        return "";
    }
}

// Return strings formatted for display
QString Strings::getStrings(){
    QString strings = "";
    for (int i = 0; i < matrixLen; i++){
        QString str = stringsMatrix[i][1];
        // Escape new line characters
        str.replace('\n', "\\n");
        strings.append(str + "\n");
    }
    return strings;
}

// Return addresses of strings formatted for display
QString Strings::getStringsAddresses(){
    QString addresses = "";
    for (int i = 0; i < matrixLen; i++){
        addresses.append(stringsMatrix[i][0] + "\n");
    }
    return addresses;
}

