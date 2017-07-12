#include "objdumper.h"
#include "QString"
#include "QStringList"
#include "QList"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

QList<int> sectionIndices;
QStringList funtionsList;
QString disassembly;
QString symbolsTable;
QString relocationEntries;
QString strings;

ObjDumper::ObjDumper(QString file)
{
    setDisassembly(file);
    setFunctionsLists(disassembly);
    setSymbolsTable(file);
    setRelocationEntries(file);
    setStrings(file);
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

// Setters

// Sets functionList and sectionIndices given disassembly text
void ObjDumper::setFunctionsLists(QString dump){
    QString tmp = "";

    for(int i = 0; i < dump.length(); i++){
        if(dump.at(i) == QChar('<')){
            i++;
            // Build function string
            while(i < dump.length()-1 && dump.at(i) != QChar('>')){
                tmp.append(dump.at(i));
                i++;
            }

            i++;    // Move to next char

            // If char after '>' is ':' add to list
            if(dump.at(i) == QChar(':')){
                funtionsList << tmp;
                sectionIndices << i;
                cout << i << endl;
            }
            // Clear tmp
            tmp = "";
        }
    }

}

void ObjDumper::setDisassembly(QString file){
    disassembly = getDump("-M intel -d", file);
}

void ObjDumper::setSymbolsTable(QString file){
    symbolsTable = getDump("-T", file);
}

void ObjDumper::setRelocationEntries(QString file){
    relocationEntries = getDump("-R", file);
}

void ObjDumper::setStrings(QString file){
    strings = getDump("-s", file);
}


// Getters

QString ObjDumper::getDisassembly(){
    return disassembly;
}

QString ObjDumper::getSymbolsTable(){
    return symbolsTable;
}

QString ObjDumper::getRelocationEntries(){
    return relocationEntries;
}

QString ObjDumper::getStrings(){
    return strings;
}

QStringList ObjDumper::getFunctionsList(){
    return funtionsList;
}

QList<int> ObjDumper::getSectionIndices(){
    return sectionIndices;
}
