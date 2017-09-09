#include "objdumper.h"
#include "QVector"
#include "QStringRef"
#include "QProcess"

#include "QDebug"

ObjDumper::ObjDumper()
{
    // Set default options
    useCustomBinary = false;
    objdumpBinary = "objdump";
    outputSyntax = "intel";
    disassemblyFlag = "-d";
    demangleFlag = "";
    target = "";
    insnwidth = 10;

    addressRegex.setPattern("[0-9a-f]+");
}

// Runs objdump given arguments and returns outout
QString ObjDumper::getDump(QStringList argsList){
    QString objdumpStr;

    if (useCustomBinary && objdumpBinary != "")
        objdumpStr = objdumpBinary;
    else
        objdumpStr = "objdump";

    QProcess *process = new QProcess(0);
    process->start(objdumpStr, argsList);

    if (!process->waitForStarted())
        return "";

    if (!process->waitForFinished())
        return "";

    QByteArray output;
    output.append(process->readAllStandardError());
    output.append(process->readAllStandardOutput());

    return output;
}

// Parses disassembly and populates function list
QVector<Function> ObjDumper::getFunctionData(QString file, QVector<QString> baseOffsets){
   QVector<Function> functionList;
   QString dump = getDisassembly(file);

    // Split dump into vector of string references to each function
    QVector<QStringRef> dumpList = dump.splitRef("\n\n");
    QString currentSection = "";

    // Parse dumplist
    for (int listIndex = 0; listIndex < dumpList.length(); listIndex++){
        QStringRef dumpStr = dumpList.at(listIndex);

        // Parse first word
        QString tmp;
        int i = 0;
        while (i < dumpStr.length()-1 && dumpStr.at(i) != QChar(' ')){
            tmp.append(dumpStr.at(i));
            i++;
        }

        // Check if section or function
        if (tmp == "Disassembly"){
            currentSection = dumpStr.mid(23).toString();
            currentSection.chop(1);

        } else if (tmp.startsWith("0") /*tmp is address*/){
            QString name = "";
            QString address = "";
            QString fileOffest = "";
            QVector< QVector<QByteArray> > functionMatrix;

            // Get function address
            address = tmp;

            // Get file offset
            fileOffest = getFileOffset(address, baseOffsets)[0];

            // Get function name
            i += 2;
            while (i < dumpStr.length()-1 && dumpStr.at(i) != QChar('\n')){
                name.append(dumpStr.at(i));
                i++;
            }
            name.chop(2);

            // Parse function contents
            QStringRef contents = dumpStr.mid(i);
            QVector<QStringRef> lines = contents.split("\n");

            for (int lineNum = 0; lineNum < lines.length()-1; lineNum++){
                QStringRef line = lines.at(lineNum);
                QVector<QByteArray> row = parseFunctionLine(line);

                if (row[0] != ""){
                    functionMatrix.append(row);
                }

            }

            // Add to functionList
            Function function(name, address, currentSection, fileOffest, functionMatrix);
            functionList.push_back(function);
        }


    }
    return functionList;

}

QVector<QByteArray> ObjDumper::parseFunctionLine(QStringRef line){
    QVector<QByteArray> row(4);

    // Get address
    QByteArray address;
    int pos = 0;

    while (pos < line.length() && line.at(pos) != QChar(':')){
        address.append(line.at(pos));
        pos++;
    }
    address = address.trimmed();

    // Note: some lines may say skipping zeros rather than containing disassembly

    QRegularExpressionMatch addressMatch = addressRegex.match(address);

    if (addressMatch.hasMatch() && addressMatch.capturedLength(0) == address.length()){
        row[0] = "0x" + address.trimmed();

        pos++;

        // Get hex
        while (pos < line.length() && (line.at(pos) == QChar(' ') || line.at(pos) == QChar('\t') )){
            pos++;
        }

        row[1] = line.mid(pos, insnwidth * 3).toLocal8Bit();
        row[1].replace(" ", "");
        int paddingLength = (insnwidth * 2) - row[1].length();
        for (int i = 0; i < paddingLength; i++){
            row[1].append(" ");
        }

        pos += insnwidth * 3;

        // Get optcode
        while (pos < line.length() && (line.at(pos) == QChar(' ') || line.at(pos) == QChar('\t') )){
            pos++;
        }
        QByteArray opt;
        while (pos < line.length() && line.at(pos) != QChar(' ')){
            opt.append(line.at(pos));
            pos++;
        }

        row[2] = opt;

        pos++;

        while (pos < line.length() && line.at(pos) == QChar(' ')){
            pos++;
        }

        // Get args
        row[3] = line.mid(pos).toLocal8Bit();

        // Remove extra space from byte array
        row[0].squeeze();
        row[1].squeeze();
        row[2].squeeze();
        row[3].squeeze();
    } else {
        row[0] = "";
        row[1] = "";
        row[2] = "";
        row[3] = "";
    }

    return row;
}

// Parses result of all contents(objdump -s) and populates section list
QVector<Section> ObjDumper::getSectionData(QString file){
    QVector<Section> sectionList;
    QString contents = getContents(file);

    QVector<QStringRef> contentsList = contents.splitRef("Contents of section ");

    // Parse contents list
    for (int listIndex = 0; listIndex < contentsList.length(); listIndex++){
        QString sectionName;
        QVector< QVector<QByteArray> > sectionMatrix;

        QStringRef contentsStr = contentsList.at(listIndex);

        // Get section name
        int i = 0;
        while (i < contentsStr.length()-1 && contentsStr.at(i) != QChar(':')){
            sectionName.append(contentsStr.at(i));
            i++;
        }

        QStringRef sectionContents = contentsStr.mid(i+2);

        // Split content into lines
        QVector<QStringRef> lines = sectionContents.split("\n");

        // Parse each line and add data to lists
        for (int lineNum = 0; lineNum < lines.length()-1; lineNum++){
            QStringRef line = lines.at(lineNum);
            QVector<QByteArray> row = parseSectionLine(line);

            sectionMatrix.append(row);
        }

        // Insert new section
        Section section(sectionName, sectionMatrix);
        sectionList.push_back(section);

    }

    return sectionList;
}

QVector<QByteArray> ObjDumper::parseSectionLine(QStringRef line){
    QVector<QByteArray> row(2);

    // Get Address
    QByteArray address;
    int pos = 1;
    while (pos < line.length() && line.at(pos) != QChar(' ')){
        address.append(line.at(pos));
        pos++;
    }
    row[0] = address;

    pos++;

    // Next 35 chars are hex followed by 2 spaces
    QByteArray hexStr = line.mid(pos, 35).toLocal8Bit();

    // Add space between each byte(default is space between 4 byte words)
    for (int i = 2; i < hexStr.length(); i+=3){
        if (hexStr.at(i) != QChar(' ')){
            hexStr.insert(i, ' ');
        }
    }

    row[1] = hexStr;

    // Ignore ascii

    // Remove extra space from byte array
    row[0].squeeze();
    row[1].squeeze();

    return row;
}

// Get file format by parsing header
QString ObjDumper::getFileFormat(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << "-f" << file;
    QString header = getDump(argsList);
    QString fileFormat = "";

    if (!header.contains("File format not recognized")){
        // Extract file format from header
        int i = 0;
        int newlineCount = 0;
        while (i < header.length() && newlineCount < 2){
            if (header.at(i) == QChar('\n'))
                newlineCount++;
            i++;
        }

        i-=2;
        while (i >= 0 && header.at(i) != QChar(' ')) {
            fileFormat.prepend(header.at(i));
            i--;
        }
    }

    return fileFormat;

}

// Get disassembly: objdump -d
QString ObjDumper::getDisassembly(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << "--insn-width=" + QString::number(insnwidth) << demangleFlag << "-M" << outputSyntax << disassemblyFlag << file;
    QString disassembly = getDump(argsList);
    return removeHeading(disassembly, 4);
}

// Get symbols table: objdump -t
QString ObjDumper::getSymbolsTable(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << demangleFlag << "-t" << file;
    QString symbolsTable = getDump(argsList);
    return removeHeading(symbolsTable, 4);
}

// Get relocation entries: objdump -R
QString ObjDumper::getRelocationEntries(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << demangleFlag << "-R" << file;
    QString relocationEntries = getDump(argsList);
    return removeHeading(relocationEntries, 4);
}

// Get all contents(hexdump of sections): objdump -s
QString ObjDumper::getContents(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << "-s" << file;
    QString contents = getDump(argsList);
    return removeHeading(contents, 3);
}

// Get archive, file and private headers: objdump -a -f -p
QString ObjDumper::getHeaders(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << "-a" << "-f" << "-p" << file;
    QString headers = getDump(argsList);
    return removeHeading(headers, 3);
}

// Try to dump a header with objdump and return any errors
QString ObjDumper::checkForErrors(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << "-f" << file;
    QString dumpStr = getDump(argsList);
    return parseDumpForErrors(dumpStr);
}

// Check for objdumps output errors. returns empty string if no errors found
QString ObjDumper::parseDumpForErrors(QString dump){
    if (dump.contains("UNKNOWN")){
        return "Architecture unknown.";
    } else if (dump.contains("File format not recognized")){
        return "Format not recognized.";
    }else if (dump.contains("File format is ambiguous")){
        QVector<QStringRef> dumpList = dump.splitRef(":");
        if (dumpList.length() == 5){
            // returns "Matching formats:[format1] [format2] [format3]..."
            return dumpList.at(3).toString() + ":" + dumpList.at(4).toString();
        }
    }

    return "";
}

// Returns base offset [base vma, base file offset]
QVector<QString> ObjDumper::getBaseOffset(QString file){
    QVector<QString> baseOffset(2);
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << "-h" << file;
    QString sectionHeader = getDump(argsList);

    if (!sectionHeader.isEmpty() && !sectionHeader.contains("File format not recognized")){
        QStringRef firstSection = sectionHeader.splitRef('\n').at(5);
        QVector<QStringRef> sectionVector = firstSection.split(' ', QString::SkipEmptyParts);
        baseOffset[0] = sectionVector.at(3).toString();
        baseOffset[1] = sectionVector.at(5).toString();
    }

    return baseOffset;
}

// Get file offset of virtual memory address as vecotor [hex value, decimal value]
QVector<QString> ObjDumper::getFileOffset(QString targetAddress, QVector<QString> baseOffsets){
    QVector<QString> fileOffset(2);
    fileOffset[0] = "";
    fileOffset[1] = "";
    bool targetAddrOk;
    bool baseAddrOk;
    bool baseOffsetOk;
    qlonglong targetAddr = targetAddress.toLongLong(&targetAddrOk, 16);
    qlonglong baseAddr = baseOffsets[0].toLongLong(&baseAddrOk, 16);
    qlonglong baseOffset = baseOffsets[1].toLongLong(&baseOffsetOk, 16);

    if (targetAddrOk && baseAddrOk && baseOffsetOk){
        if (targetAddr >= baseAddr){
            qlonglong targetOffset = (targetAddr - baseAddr) + baseOffset;
            fileOffset[0] = "0x" + QString::number(targetOffset, 16);
            fileOffset[1] = QString::number(targetOffset);
        }
    }

    return fileOffset;
}

// Removes heading(first [numLines] lines of objdump output)
QString ObjDumper::removeHeading(QString dump, int numLines){
    int i = 0;
    int newlineCount = 0;
    while (i < dump.length() && newlineCount < numLines){
        if (dump.at(i) == QChar('\n'))
            newlineCount++;
        i++;
    }

    return dump.mid(i);
}

// Returns first [numLines] of given string
QString ObjDumper::getHeading(QString dump, int numLines){
    int i = 0;
    int newlineCount = 0;
    while (i < dump.length() && newlineCount < numLines){
        if (dump.at(i) == QChar('\n'))
            newlineCount++;
        i++;
    }

    return dump.left(i);
}

//  Set Options
void ObjDumper::setUseCustomBinary(bool useCustom){
    useCustomBinary = useCustom;
}

void ObjDumper::setobjdumpBinary(QString binary){
    objdumpBinary = binary;
}

void ObjDumper::setOutputSyntax(QString syntax){
    outputSyntax = syntax;
}

void ObjDumper::setDisassemblyFlag(QString flag){
    disassemblyFlag = flag;
}

void ObjDumper::setDemangleFlag(QString flags){
    demangleFlag = flags;
}

// Set target flag: "-b [target]"
void ObjDumper::setTarget(QString trgt){
    target = trgt;
}
