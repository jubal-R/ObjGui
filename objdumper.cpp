#include "objdumper.h"
#include "QString"
#include "QStringList"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

ObjDumper::ObjDumper()
{

}

QString ObjDumper::getDump(QString args, QString file){
    ostringstream oss;
    FILE *in;
    char buff[100];
    string cmd = "objdump " + args.toStdString() + " " + file.toStdString();

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

QStringList ObjDumper::getFunctionsList(QString dump){
    QStringList output;
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
                output << tmp;
            }
            // Clear tmp
            tmp = "";
        }
    }
    return output;
}

QString ObjDumper::getDisassembly(QString file){
    return getDump("-M intel -d", file);
}

QString ObjDumper::getSymbolsTable(QString file){
    return getDump("-T", file);
}

QString ObjDumper::getRelocationEntries(QString file){
    return getDump("-R", file);
}

QString ObjDumper::getStrings(QString file){
    return getDump("-s", file);
}
