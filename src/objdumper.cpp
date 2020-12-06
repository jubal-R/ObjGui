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
    hexBytesRegex.setPattern("[0-9a-f ]+");
}

// Runs objdump given arguments and returns outout
QByteArray ObjDumper::getDump(QStringList argsList){
    QString objdumpStr;

    if (useCustomBinary && !objdumpBinary.isEmpty())
        objdumpStr = objdumpBinary;
    else
        objdumpStr = QStringLiteral("objdump");

    QProcess *process = new QProcess(0);
    process->start(objdumpStr, argsList);

    if (!process->waitForStarted())
        return "";

    if (!process->waitForFinished())
        return "";

    QByteArray output;
    output.reserve(process->bytesAvailable());
    output.append(process->readAllStandardError());
    output.append(process->readAllStandardOutput());

    return output;
}

// Parses disassembly and populates function list
QVector<Function> ObjDumper::getFunctionData(QString file, QVector<QString> baseOffsets) {
    QVector<Function> functionList;

    const QByteArray dump = getDisassembly(file);
    int idx = dump.indexOf(QByteArrayLiteral("\n\n"));
    int prev = 0;
    QString currentSection;
    while (idx != -1) {

        // Parse first word
        int p = dump.indexOf(' ', prev);
        QByteArray tmp = dump.mid(prev, p - prev);

        // Check if section or function
        if (tmp == "Disassembly") {
            currentSection = dump.mid(prev + 23, idx - (prev + 23));

        } else if (tmp.startsWith('0') /*tmp is address*/) {
            int i = prev;
            QString address;
            QString fileOffest;
            QVector< std::array<QByteArray, 5> > functionMatrix;

            // Get function address
            address = std::move(tmp);

            // Get file offset
            fileOffest = getFileOffset(address, baseOffsets)[0];

            // Get function name
            i += 2 + address.size();
            int endlinePos = dump.indexOf('\n', i);
            QString name = dump.mid(i, endlinePos - i - 2);
            i = endlinePos;

            // Parse function contents

            //we have to move forward, going through all the \n till we reach idx
            int npos = dump.indexOf('\n', i);
            int nprev = i;
            while (npos < idx) {
                auto row = parseFunctionLine(dump, nprev, npos - nprev);
                if (!row.at(0).isEmpty()){
                    functionMatrix.append(std::move(row));
                }
                nprev = npos + 1;
                npos = dump.indexOf('\n', nprev);
            }

            // Add to functionList
            Function function(std::move(name),
                              std::move(address),
                              std::move(currentSection),
                              std::move(fileOffest),
                              std::move(functionMatrix));
            functionList.push_back(std::move(function));
        }
        prev = idx + 2; // +2 to skip \n\n
        idx = dump.indexOf("\n\n", prev);
    }

    return functionList;
}

std::array<QByteArray, 5> ObjDumper::parseFunctionLine(const QByteArray &line, int posInDump, int lineSize)
{
    std::array<QByteArray, 5> row;
    if (lineSize > insnwidth * 3) {
        int pos = posInDump;
        int size = pos + lineSize;

        // Get address
        int addressStart = pos;
        while (pos < size && line.at(pos) != QChar(':')){
            pos++;
        }

        row[0] = parseAddress(line, addressStart, pos - addressStart);

        pos++;


        // Skip whitespace
        while (pos < size && (line.at(pos) == QChar(' ') || line.at(pos) == QChar('\t') )){
            pos++;
        }

        // Get hex
        row[1] = parseHexBytes(line, pos, insnwidth * 3);

        pos += insnwidth * 3;

        // Skip whitespace
        while (pos < size && (line.at(pos) == QChar(' ') || line.at(pos) == QChar('\t') )){
            pos++;
        }

        // Get optcode
        QByteArray opt;
        opt.reserve(4);
        while (pos < size && line.at(pos) != QChar(' ')){
            opt.append(line.at(pos));
            pos++;
        }
        pos++;

        row[2] = std::move(opt);

        while (pos < size && line.at(pos) == QChar(' ')){
            pos++;
        }

        // Get args
        auto len = size - pos;
        len = len == -1 ? 0 : len;
        row[3] = line.mid(pos, len);

        // empty by default, used for xref data
        row[4] = "";
    } else {
        row[0] = "";
        row[1] = "";
        row[2] = "";
        row[3] = "";
        row[4] = "";
    }

    return row;
}

static inline bool ishex(char c) {
    return isdigit(c) || (c >= 97 && c <= 102);
}

QByteArray ObjDumper::parseAddress(const QByteArray& address, int pos, int size){

    int i = pos;
    for (; i < (pos + size); ++i) {
        if (!isspace(address.at(i)) ) {
            break;
        }
    }

    QByteArray ret = QByteArrayLiteral("0x");
    ret.reserve(2 + 6);
    int start = i;
    while (i < (pos + size) && ishex(address.at(i))) {
        ret.append(address.at(i));
        i++;
    }

    if (start == i)
        return QByteArrayLiteral("");
    return ret;
}

QByteArray ObjDumper::parseHexBytes(const QByteArray& byteString, int pos, int size) {
    QByteArray r1;
    //"e8 3c ed 03 00                "
    r1.reserve(size - insnwidth);
    for (int i = pos; i < (pos + size); i += 3) {
        char hi = byteString.at(i);
        char lo = byteString.at(i + 1);
        if (ishex(hi) && ishex(lo)) {
            r1.append(hi);
            r1.append(lo);
        } else if (isspace(hi)) {
            break;
        }
        //space skipped
    }
    //add padding
    int paddingLength = (insnwidth * 2) - r1.length();
    r1.append(paddingLength, ' ');

    return r1;
}

// Parses result of all contents(objdump -s) and populates section list
QVector<Section> ObjDumper::getSectionData(QString file){
    QVector<Section> sectionList;

    const QByteArray contents = getContents(file);
    int sectionPos = contents.indexOf("Contents of section ");
    int prevSection = 0;
    while (sectionPos != -1) {
        int i = sectionPos + 20;
        int colonPos = contents.indexOf(':', i);
        QString sectionName = contents.mid(i, colonPos - i);

        QVector< std::array<QByteArray, 2> > sectionMatrix;

        i = colonPos + 2;
        //move forward iterating over each line '\n'
        int npos = contents.indexOf('\n', i);
        int prevN = i;
        int nextSectionPos = contents.indexOf("Contents of section ", sectionPos + 20);
        nextSectionPos = nextSectionPos == -1 ? contents.length() : nextSectionPos;
        while (npos < nextSectionPos && npos != -1) {
            auto row = parseSectionLine(contents, prevN, npos - prevN);
            if (!row.at(0).isEmpty()){
                sectionMatrix.append(std::move(row));
            }
            prevN = npos + 1;
            npos = contents.indexOf('\n', prevN);
        }

        // Insert new section
        Section section(std::move(sectionName), std::move(sectionMatrix));
        sectionList.push_back(std::move(section));

        prevSection = sectionPos + 21;
        sectionPos = contents.indexOf("Contents of section ", prevSection);
    }
    return sectionList;
}

std::array<QByteArray, 2> ObjDumper::parseSectionLine(const QByteArray &line, int posInLine, int lineSize)
{
    std::array<QByteArray, 2> row;

    // Get Address
    QByteArray address;
    address.reserve(6);
    int pos = posInLine + 1;
    int len = posInLine + lineSize;
    while (pos < len && line.at(pos) != QChar(' ')){
        address.append(line.at(pos));
        pos++;
    }

    row[0] = std::move(address);

    pos++;

    char hexCStr[48];
    int j = 0;
    for (int i = pos; i < pos + 35; i+=8) {
        hexCStr[j++] = line[i];
        hexCStr[j++] = line[i + 1];
        hexCStr[j++] = ' ';

        hexCStr[j++] = line[i + 2];
        hexCStr[j++] = line[i + 3];
        hexCStr[j++] = ' ';

        hexCStr[j++] = line[i + 4];
        hexCStr[j++] = line[i + 5];
        hexCStr[j++] = ' ';

        hexCStr[j++] = line[i + 6];
        hexCStr[j++] = line[i + 7];
        hexCStr[j++] = ' ';
        i++;
    }

    QByteArray hexStr(hexCStr, 47);

    row[1] = std::move(hexStr);

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
QByteArray ObjDumper::getDisassembly(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << "--insn-width=" + QString::number(insnwidth) << demangleFlag << "-M" << outputSyntax << disassemblyFlag << file;
    QByteArray disassembly = getDump(argsList);
    return removeHeading(disassembly, 4);
}

// Get symbols table: objdump -t
QString ObjDumper::getSymbolsTable(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << demangleFlag << "-t" << file;
    QByteArray symbolsTable = getDump(argsList);
    return removeHeading(symbolsTable, 4);
}

// Get relocation entries: objdump -R
QString ObjDumper::getRelocationEntries(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << demangleFlag << "-R" << file;
    QByteArray relocationEntries = getDump(argsList);
    return removeHeading(relocationEntries, 4);
}

// Get all contents(hexdump of sections): objdump -s
QByteArray ObjDumper::getContents(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << "-s" << file;
    QByteArray contents = getDump(argsList);
    return removeHeading(contents, 3);
}

// Get archive, file and private headers: objdump -a -f -p
QString ObjDumper::getHeaders(QString file){
    QStringList argsList;
    if (!target.isEmpty())
        argsList << target;
    argsList << "-a" << "-f" << "-p" << file;
    QByteArray headers = getDump(argsList);
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
        QVector<QStringRef> headerLines = sectionHeader.splitRef('\n');

        if (headerLines.size() > 5) {
            QStringRef firstSection = headerLines.at(5);

            QVector<QStringRef> sectionVector = firstSection.split(' ', QString::SkipEmptyParts);

            if (sectionVector.size() > 5){
                QString baseAddress = sectionVector.at(3).toString();
                QString baseFileOffset = sectionVector.at(5).toString();

                QRegularExpressionMatch addressMatch = addressRegex.match(baseAddress);
                QRegularExpressionMatch fileOffsetMatch = addressRegex.match(baseFileOffset);

                if (addressMatch.hasMatch() && fileOffsetMatch.hasMatch()){
                    baseOffset[0] = baseAddress;
                    baseOffset[1] = baseFileOffset;

                    return baseOffset;
                }
            }
        }
    }

    baseOffset[0] = "000000";
    baseOffset[1] = "000000";

    return baseOffset;
}

// Get file offset of virtual memory address as vecotor [hex value, decimal value]
QVector<QString> ObjDumper::getFileOffset(QString targetAddress, const QVector<QString> &baseOffsets){
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
            fileOffset[0] = QStringLiteral("0x") + QString::number(targetOffset, 16);
            fileOffset[1] = QString::number(targetOffset);
        }
    }

    return fileOffset;
}

// Removes heading(first [numLines] lines of objdump output)
QByteArray ObjDumper::removeHeading(QByteArray dump, int numLines){
    int i = 0;
    int newlineCount = 0;
    while (i < dump.length() && newlineCount < numLines){
        if (dump.at(i) == '\n')
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
