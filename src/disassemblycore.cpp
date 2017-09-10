#include "disassemblycore.h"

#include "QtConcurrent/QtConcurrent"
#include "QFuture"
#include "QRegularExpression"

#include "QDebug"

DisassemblyCore::DisassemblyCore()
{
    fileLoaded = false;
}

/*
 *  Disassembly
*/

QVector<QString> DisassemblyCore::getBaseOffsets(){
    return baseOffsets;
}

void DisassemblyCore::disassemble(QString file){
    baseOffsets = objDumper.getBaseOffset(file);

    functionData.clear();
    QFuture< QVector<Function> > futureFunctionData = QtConcurrent::run(&objDumper, &ObjDumper::getFunctionData, file, baseOffsets);
    sectionData.clear();
    QFuture< QVector<Section> > futureSectionData = QtConcurrent::run(&objDumper, &ObjDumper::getSectionData, file);
    QFuture<QVector< QVector<QString> > > futureStrings = QtConcurrent::run(&stringsDumper, &StringsDumper::dumpStrings, file, baseOffsets);

    functionData = futureFunctionData.result();
    sectionData = futureSectionData.result();
    strings.setStringsData(futureStrings.result());

    xrefStrings();

    fileLoaded = true;
}

bool DisassemblyCore::disassemblyIsLoaded(){
    if (fileLoaded && !functionData.isEmpty() && !sectionData.isEmpty())
        return true;
    else
        return false;
}

void DisassemblyCore::xrefStrings(){
    for (QVector<Function>::iterator itr = functionData.begin(); itr != functionData.end(); itr++){
        int functionLen = itr->getMatrixLen();

        for (int lineNum = 0; lineNum < functionLen; lineNum++){
            QString optStr = itr->getLine(lineNum)[3];

            QString address = extractAddress(optStr);

            if (address != ""){
                int strIndex = strings.getIndexByAddress(address);

                if (strIndex > 0){
                    QString str = strings.getStringAt(strIndex);
//                    qDebug() << str;

                    if(str != ""){
                        itr->setXrefData(lineNum, str);
                    }
                }
            }
        }
    }
}

QString DisassemblyCore::extractAddress(QString s){
    QRegularExpression addressRegex;
    addressRegex.setPattern("(0x)?[0-9a-f]{4,}");
    QRegularExpressionMatch regexMatch;

    if (s.contains(addressRegex, &regexMatch)){
        QString address = regexMatch.captured();
        return address;
    }

    return "";
}

QString DisassemblyCore::getObjdumpErrorMsg(QString file){
    return objDumper.checkForErrors(file);
}

QString DisassemblyCore::getSymbolsTable(QString file){
    return objDumper.getSymbolsTable(file);
}

QString DisassemblyCore::getRelocationEntries(QString file){
    return objDumper.getRelocationEntries(file);
}

QString DisassemblyCore::getHeaders(QString file){
    return objDumper.getHeaders(file);
}

QString DisassemblyCore::getFileFormat(QString file){
    return objDumper.getFileFormat(file);
}

QVector<QString> DisassemblyCore::getFileOffset(QString targetAddress){
    return ObjDumper::getFileOffset(targetAddress, baseOffsets);
}

/*
 * Disassembly Options
*/

void DisassemblyCore::setUseCustomBinary(bool useCustom){
    objDumper.setUseCustomBinary(useCustom);
}

void DisassemblyCore::setobjdumpBinary(QString binary){
    objDumper.setobjdumpBinary(binary);
}

void DisassemblyCore::setOutputSyntax(QString syntax){
    objDumper.setOutputSyntax(syntax);
}

void DisassemblyCore::setDisassemblyFlag(QString flag){
    objDumper.setDisassemblyFlag(flag);
}

void DisassemblyCore::setDemangleFlag(QString flag){
    objDumper.setDemangleFlag(flag);
}

void DisassemblyCore::setTarget(QString trgt){
    objDumper.setTarget(trgt);
}

/*
 * Function Data
*/

QStringList DisassemblyCore::getFunctionNames(){
    QStringList functionNames;
    int numFunctions = functionData.length();

    if (numFunctions > 0){
        for (int i = 0; i < numFunctions; i++){
            Function function = functionData.at(i);
            QString name = function.getName();
            functionNames.append(name);
        }
    }

    return functionNames;
}

Function DisassemblyCore::getFunction(QString name){
    int numFunctions = functionData.length();

    for (int i = 0; i < numFunctions; i++){
        Function currentFunction = functionData.at(i);
        QString currentName = currentFunction.getName();
        if (currentName == name)
            return currentFunction;
    }

    Function function;
    return function;
}

Function DisassemblyCore::getFunction(int index){
    int numFunctions = functionData.length();

    if (numFunctions > 0){
        Function function = functionData.at(index);
        return function;
    }

    Function function;
    return function;
}

int DisassemblyCore::getFunctionIndex(QString functionName){
    int numFunctions = functionData.length();

    for (int i = 0; i < numFunctions; i++){
        Function currentFunction = functionData.at(i);
        QString currentName = currentFunction.getName();
        if (currentName == functionName)
            return i;
    }

    return -1;
}

bool DisassemblyCore::functionExists(QString name){
    int numFunctions = functionData.length();

    for (int i = 0; i < numFunctions; i++){
        Function currentFunction = functionData.at(i);
        QString currentName = currentFunction.getName();
        if (currentName == name)
            return true;
    }

    return false;
}

// Find location of target address and return what function its in and its location within the function [functionIndex, functionMatrixIndex]
QVector<int> DisassemblyCore::getAddressLocation(QString targetAddress){
    QVector<int> location(2);

    // Search each function
    for (int functionIndex = 0; functionIndex < functionData.length(); functionIndex++){
        Function function = functionData.at(functionIndex);

        // Check if matrix is empty
        if (function.getMatrixLen() > 0){
            // Binary search
            int upperLimit = function.getMatrixLen() - 1;
            int lowerLimit = 0;
            int currentIndex = upperLimit / 2;

            while (lowerLimit < upperLimit && currentIndex != 0){
                QString currentAddress = function.getAddressAt(currentIndex);

                if (currentAddress == targetAddress){
                    location[0] = functionIndex;
                    location[1] = currentIndex;
                    return location;
                } else if (targetAddress < currentAddress){
                    upperLimit = currentIndex - 1;
                    currentIndex = (upperLimit + lowerLimit) / 2;
                } else {
                    lowerLimit = currentIndex + 1;
                    currentIndex = (upperLimit + lowerLimit) / 2;
                }

            }
            // Final checks
            if (function.getAddressAt(currentIndex) == targetAddress){
                location[0] = functionIndex;
                location[1] = currentIndex;
                return location;
            } else if (function.getMatrixLen() > 1 && function.getAddressAt(1) == targetAddress){
                location[0] = functionIndex;
                location[1] = 1;
                return location;
            }
        }


    }

    // If not found return [-1,-1]
    location[0] = -1;
    location[1] = -1;

    return location;
}

// Find all calls to a target function and return list of each calling function and address of call
QVector< QVector<QString> > DisassemblyCore::findCallsToFunction(QString targetFunction){
    QVector< QVector<QString> > results;
    targetFunction = "<" + targetFunction + ">";

    // Search each function
    for (int functionIndex = 0; functionIndex < functionData.length(); functionIndex++){
        Function function = functionData.at(functionIndex);

        // Check if matrix is empty
        if (function.getMatrixLen() > 0){
            // Search function
            int matrixLen = function.getMatrixLen();
            for (int i = 0; i < matrixLen; i++){
                QVector<QByteArray> line = function.getLine(i);
                // Check for call to target function
                if (line[2] == "call" && QString::fromLocal8Bit(line[3]).contains(targetFunction)){
                    QVector<QString> result(2);
                    result[0] = function.getName();
                    result[1] = line[0];
                    results.append(result);
                }
            }

        }
    }

    return results;
}

// Find all references to a target string
QVector< QVector<QString> > DisassemblyCore::findReferences(QString target){
    QVector< QVector<QString> > results;

    // Search each function
    for (int functionIndex = 0; functionIndex < functionData.length(); functionIndex++){
        Function function = functionData.at(functionIndex);

        // Check if matrix is empty
        if (function.getMatrixLen() > 0){
            // Search function
            int matrixLen = function.getMatrixLen();
            for (int i = 0; i < matrixLen; i++){
                QVector<QByteArray> line = function.getLine(i);
                // Check for call to target function
                if (QString::fromLocal8Bit(line[3]).contains(target)){
                    QVector<QString> result(2);
                    result[0] = function.getName();
                    result[1] = line[0];
                    results.append(result);
                }
            }

        }
    }

    return results;
}

/*
 *  Section Data
*/

QString DisassemblyCore::getSectionHexDump(){
    int len = sectionData.length();
    QString hexStr;

    for (int i = 1; i < len; i++){
        Section section = sectionData.at(i);
        hexStr.append(section.getSectionName() + "\n" + section.getHexString() + "\n");
    }

    return hexStr;
}

QString DisassemblyCore::getSectionAddressDump(){
    int len = sectionData.length();
    QString addressStr;

    for (int i = 1; i < len; i++){
        Section section = sectionData.at(i);
        addressStr.append("\n" + section.getAddressString() + "\n");
    }

    return addressStr;
}

/*
 *  Strings Data
*/

QString DisassemblyCore::getStringsAddresses(){
    return strings.getStringsAddresses();
}

QString DisassemblyCore::getStrings(){
    return strings.getStrings();
}

int DisassemblyCore::getStringIndexByAddress(QString address){
    return strings.getIndexByAddress(address);
}

QString DisassemblyCore::getStringAddressAt(int index){
    return strings.getAddressAt(index);
}
