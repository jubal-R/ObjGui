#include "disassemblycore.h"

#include "QtConcurrent/QtConcurrent"
#include "QFuture"

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

    functionList.nukeList();
    QFuture<FunctionList> futureFunctionList = QtConcurrent::run(&objDumper, &ObjDumper::getFunctionList, file, baseOffsets);
    sectionList.nukeList();
    QFuture<SectionList> futureSectionList = QtConcurrent::run(&objDumper, &ObjDumper::getSectionList, file);
    QFuture<QVector< QVector<QString> > > futureStrings = QtConcurrent::run(&stringsDumper, &StringsDumper::dumpStrings, file, baseOffsets);

    functionList = futureFunctionList.result();
    sectionList = futureSectionList.result();
    strings.setStringsData(futureStrings.result());

    fileLoaded = true;
}

bool DisassemblyCore::disassemblyIsLoaded(){
    if (fileLoaded && !functionList.isEmpty() && !sectionList.isEmpty())
        return true;
    else
        return false;
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

void DisassemblyCore::setArchiveHeaderFlag(QString flag){
    objDumper.setArchiveHeaderFlag(flag);
}

void DisassemblyCore::setFileHeaderFlag(QString flag){
    objDumper.setFileHeaderFlag(flag);
}

void DisassemblyCore::setPrivateHeaderFlag(QString flag){
    objDumper.setPrivateHeaderFlag(flag);
}

void DisassemblyCore::setSectionsHeaderFlag(QString flag){
    objDumper.setSectionsHeaderFlag(flag);
}

/*
 * Function Data
*/

QStringList DisassemblyCore::getFunctionNames(){
    return functionList.getFunctionNames();
}

Function DisassemblyCore::getFunction(QString name){
    return functionList.getFunction(name);
}

Function DisassemblyCore::getFunction(int index){
    return functionList.getFunction(index);
}

int DisassemblyCore::getFunctionIndex(QString functionName){
    return functionList.getFunctionIndex(functionName);
}

bool DisassemblyCore::functionExists(QString name){
    return functionList.containsFunction(name);
}

// Find location of target address and return what function its in and its location within the function [functionIndex, functionMatrixIndex]
QVector<int> DisassemblyCore::getAddressLocation(QString targetAddress){
    QVector<int> location(2);

    // Search each function
    for (int functionIndex = 0; functionIndex < functionList.getLength(); functionIndex++){
        Function function = functionList.getFunction(functionIndex);

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
    for (int functionIndex = 0; functionIndex < functionList.getLength(); functionIndex++){
        Function function = functionList.getFunction(functionIndex);

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
    for (int functionIndex = 0; functionIndex < functionList.getLength(); functionIndex++){
        Function function = functionList.getFunction(functionIndex);

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
    int len = sectionList.getLength();
    QString hexStr;

    for (int i = 1; i < len; i++){
        Section section = sectionList.getSection(i);
        hexStr.append(section.getSectionName() + "\n" + section.getHexString() + "\n");
    }

    return hexStr;
}

QString DisassemblyCore::getSectionAddressDump(){
    int len = sectionList.getLength();
    QString addressStr;

    for (int i = 1; i < len; i++){
        Section section = sectionList.getSection(i);
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
