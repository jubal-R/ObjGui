#include "objdumper.h"
#include "QVector"
#include "QStringRef"
#include <stdlib.h>
#include <fstream>
#include <sstream>

#include "QDebug"

ObjDumper::ObjDumper()
{
    // Set default options
    useCustomBinary = false;
    objdumpBinary = "objdump";
    outputSyntax = "intel";
    disassemblyFlag = "-d";
    headerFlags = "-a -f -p -h";
    optionalFlags = "";
    target = "";
    insnwidth = 10;

    addressRegex.setPattern("[0-9a-f]+");
}

// Runs objdump given arguments and file then returns outout
QString ObjDumper::getDump(QString args, QString file){
    std::ostringstream oss;
    FILE *in;
    char buff[100];
    std::string objdumpStr;

    if (useCustomBinary && objdumpBinary != "")
        objdumpStr = objdumpBinary.toStdString();
    else
        objdumpStr = "objdump";

    std::string cmd = "\"" + objdumpStr + "\" "  + target.toStdString() + " " + args.toStdString() + " \"" + file.toStdString() + "\" 2>&1";

    try{
        if(!(in = popen(cmd.c_str(),"r") )){
                return "Failed to get object dump.";
            }
            while(fgets(buff, sizeof(buff), in) !=NULL){
                oss << buff;
            }
            pclose(in);

    }catch(const std::exception& e){
        return "Something went wrong.";
    }catch (const std::string& ex) {

    } catch (...) {

    }

    return QString::fromStdString(oss.str());
}

// Parses disassembly and populates functionList
FunctionList ObjDumper::getFunctionList(QString file, QVector<QString> baseOffsets){
   FunctionList functionList;
   QString dump = getDisassembly(file);

    // Split dump into list of functions
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
                QVector<QByteArray> row(4);

                // Get address
                QByteArray address;
                int pos = 0;
                // Get address
                while (pos < line.length() && line.at(pos) != QChar(':')){
                    address.append(line.at(pos));
                    pos++;
                }
                address = address.trimmed();

                // Validate address
                QRegularExpressionMatch addressMatch = addressRegex.match(address);

                if (addressMatch.hasMatch() && addressMatch.capturedLength(0) == address.length()){
                    row[0] = address.trimmed();

                    pos++;

                    // Get hex
                    while (pos < line.length() && (line.at(pos) == QChar(' ') || line.at(pos) == QChar('\t') )){
                        pos++;
                    }

                    row[1] = line.mid(pos, insnwidth * 3).toLocal8Bit();
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

                    // Get args
                    row[3] = line.mid(pos).toLocal8Bit();

                    // Remove extra space from byte array
                    row[0].squeeze();
                    row[1].squeeze();
                    row[2].squeeze();
                    row[3].squeeze();

                    functionMatrix.append(row);
                }
            }

            // Add to functionList
            functionList.insert(name, address, currentSection, fileOffest, functionMatrix);
        }


    }
    return functionList;

}

SectionList ObjDumper::getSectionList(QString file){
    SectionList sectionList;
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

            sectionMatrix.append(row);

        }

        // Insert new section
        sectionList.insert(sectionName, sectionMatrix);

    }

    return sectionList;
}


QString ObjDumper::getDisassembly(QString file){
    QString disassembly = getDump("--insn-width=" + QString::number(insnwidth) + " " +optionalFlags + " -M " + outputSyntax + " " + disassemblyFlag, file);
    // Check first few lines for errors
    QString errors = parseDumpForErrors(getHeading(disassembly, 10));   // Output formatting can differ so check more lines to be safe
    if (errors == "")
        return removeHeading(disassembly, 4);
    else
        return errors;
}

QString ObjDumper::getSymbolsTable(QString file){
    QString symbolsTable = getDump(optionalFlags + " -T", file);
    return removeHeading(symbolsTable, 4);
}

QString ObjDumper::getRelocationEntries(QString file){
    QString relocationEntries = getDump(optionalFlags + " -R", file);
    return removeHeading(relocationEntries, 4);
}

QString ObjDumper::getContents(QString file){
    QString contents = getDump("-s", file);
    return removeHeading(contents, 3);
}

QString ObjDumper::getHeaders(QString file){
    if (!headerFlags.isEmpty()){
        QString headers = getDump(headerFlags, file);
        return removeHeading(headers, 3);
    } else {
        return "";
    }
}

QString ObjDumper::getFileFormat(QString file){
    QString header = getDump("-f", file);
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

// Returns base offset [base vma, base file offset]
QVector<QString> ObjDumper::getBaseOffset(QString file){
    QVector<QString> baseOffset(2);
    QString sectionHeader = getDump("-h", file);

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
    bool targetAddrOk;
    bool baseAddrOk;
    bool baseOffsetOk;
    qlonglong targetAddr = targetAddress.toLongLong(&targetAddrOk, 16);
    qlonglong baseAddr = baseOffsets[0].toLongLong(&baseAddrOk, 16);
    qlonglong baseOffset = baseOffsets[1].toLongLong(&baseOffsetOk, 16);

    if (targetAddrOk && baseAddrOk && baseOffsetOk){
       qlonglong targetOffset = (targetAddr - baseAddr) + baseOffset;
       fileOffset[0] = "0x" + QString::number(targetOffset, 16);
       fileOffset[1] = QString::number(targetOffset);
    }

    return fileOffset;
}

// Try to dump a header with objdump and return any errors
QString ObjDumper::checkForErrors(QString file){
    QString dumpStr = getDump("-f", file);
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

// Removes heading(first three lines of objdump output)
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

void ObjDumper::setHeaderFlags(QString flags){
    headerFlags = flags;
}

void ObjDumper::setOptionalFlags(QString flags){
    optionalFlags = flags;
}

// Set target flag: "-b [target]"
void ObjDumper::setTarget(QString trgt){
    target = trgt;
}
