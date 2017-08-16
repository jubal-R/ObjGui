#include "objdumper.h"
#include "function.h"
#include "functionlist.h"
#include "QString"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

ObjDumper::ObjDumper()
{
    // Set default options
    outputSyntax = "intel";
    disassemblyFlag = "-d";
    headerFlags = "-x";
    optionalFlags = "";
}

// Runs objdump given arguments and file then returns outout
QString ObjDumper::getDump(QString args, QString file){
    ostringstream oss;
    FILE *in;
    char buff[100];
    string cmd = "objdump " + args.toStdString() + " " + file.toStdString() + " 2>&1";

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
FunctionList ObjDumper::getFunctionList(QString file){
   FunctionList functionList;
   QString dump = getDisassembly(file);

    // Split dump into list of functions
    QStringList dumpList = dump.split("\n\n");
    QString currentSection = "";

    // Parse dumplist
    for (int listIndex = 0; listIndex < dumpList.length(); listIndex++){
        QString dumpStr = dumpList.at(listIndex);

        // Parse first word
        QString tmp;
        int i = 0;
        while (i < dumpStr.length()-1 && dumpStr.at(i) != QChar(' ')){
            tmp.append(dumpStr.at(i));
            i++;
        }

        // Check if section or function
        if (tmp == "Disassembly"){
            currentSection = dumpStr.mid(23);
            currentSection.chop(1);

        } else if (tmp.startsWith("0") /*tmp is hex*/){
            QString name = "";
            QString address = "";
            QString contents = "";

            address = tmp;

            i += 2;
            QString tmp2 = "";
            while (i < dumpStr.length()-1 && dumpStr.at(i) != QChar('>')){
                tmp2.append(dumpStr.at(i));
                i++;
            }
            name = tmp2;
            contents = dumpStr.mid(i+3);


            // Add to functionList
            functionList.insert(name, address, contents, currentSection);
        }


    }
    return functionList;

}

SectionList ObjDumper::getSectionList(QString file){
    SectionList sectionList;
    QString contents = getContents(file);

    QStringList contentsList = contents.split("Contents of section ");

    // Parse contents list
    for (int listIndex = 0; listIndex < contentsList.length(); listIndex++){
        QString sectionName;
        QStringList addresses;
        QStringList hex;
        QStringList ascii;
        QString contentsStr = contentsList.at(listIndex);

        // Get section name
        QString tmp;
        int i = 0;
        while (i < contentsStr.length()-1 && contentsStr.at(i) != QChar(':')){
            tmp.append(contentsStr.at(i));
            i++;
        }
        sectionName = tmp;
        contentsStr = contentsStr.mid(i+2);

        // Split content into lines
        QStringList lines = contentsStr.split("\n");

        // Parse each line and add data to lists
        for (int lineNum = 0; lineNum < lines.length()-1; lineNum++){
            QString line = lines.at(lineNum);

            QString address;
            int pos = 1;
            while (pos < line.length() && line.at(pos) != QChar(' ')){
                address.append(line.at(pos));
                pos++;
            }
            addresses.append(address);

            pos++;

            // Next 35 chars are hex followed by 2 spaces
            QString hexStr = line.mid(pos, 35);

            // Add space between each byte(default is space between 4 byte words)
            for (int i = 2; i < hexStr.length(); i+=3){
                if (hexStr.at(i) != QChar(' ')){
                    hexStr.insert(i, ' ');
                }
            }

            hex.append(hexStr);

            pos += 37;

            ascii.append(line.mid(pos));

        }

        // Insert new section
        sectionList.insert(sectionName, addresses, hex, ascii);

    }

    return sectionList;
}


QString ObjDumper::getDisassembly(QString file){
    QString disassembly = getDump(optionalFlags + " -M " + outputSyntax + " " + disassemblyFlag, file);
    return removeHeading(disassembly, 4);
}

QString ObjDumper::getSymbolsTable(QString file){
    QString symbolsTable = getDump("-T", file);
    return removeHeading(symbolsTable, 3);
}

QString ObjDumper::getRelocationEntries(QString file){
    QString relocationEntries = getDump("-R", file);
    return removeHeading(relocationEntries, 3);
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
    // Extract file format from header
    int i = 0;
    int newlineCount = 0;
    while (i < header.length() && newlineCount < 2){
        if (header.at(i) == QChar('\n'))
            newlineCount++;
        i++;
    }
    i-=2;
    QString fileFormat = "";
    while (i >= 0 && header.at(i) != QChar(' ')) {
        fileFormat.prepend(header.at(i));
        i--;
    }

    return fileFormat;

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
