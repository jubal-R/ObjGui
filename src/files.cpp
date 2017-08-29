#include "files.h"
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include "QVector"
#include "QFile"

#include "QDebug"

Files::Files()
{
    currentDirectory = getHomeDir();
}

// Extract strings from file
QVector< QVector<QByteArray> > Files::strings(QString filename){
    QVector< QVector<QByteArray> > stringsData;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return stringsData;

    // Read each byte
    while (!file.atEnd()) {
        QByteArray bytes = file.read(1);
        char byte = bytes[0];

        // If read byte is a printable character start builing string
        if (isPrintableChar(byte)){
            qint64 pos = file.pos();
            QByteArray str = bytes;

            // Build string until running into a nonprintable character
            while (!file.atEnd()){
                bytes = file.read(1);
                byte = bytes[0];
                if (isPrintableChar(byte)){
                    str.append(byte);
                } else {
                    break;
                }
            }

            // Add built string to results if it meets length requirement
            if(str.length() >= 4){
                QVector<QByteArray> stringData(2);
                stringData[0] = QByteArray::number(pos);
                stringData[1] = str;

                stringsData.append(stringData);
            }
        }
    }

    return stringsData;
}

bool Files::isPrintableChar(char c){
    if (c >= 32 && c <= 126)
        return true;
    else
        return false;
}

//  Return Users Home Directory
QString Files::getHomeDir(){
    std::ostringstream oss;
    FILE *in;
    char buff[100];
    if(!(in = popen("echo ~/" ,"r") )){
        return "Fail :(";
    }
    while(fgets(buff, sizeof(buff), in) !=NULL){
        oss << buff;
    }
    pclose(in);

    return QString::fromStdString(oss.str().substr(0, oss.str().length()-1));
}

// Open directory in file manager
void Files::openFileManager(QString dir){
    // xdg-open + dir
    std::ostringstream oss;
    FILE *in;
    char buff[100];
    std::string cmd = "xdg-open " + dir.toStdString() + " 2>&1";

    try{
        if(!(in = popen(cmd.c_str(),"r") )){
            // Failed to open file manager
        }
        while(fgets(buff, sizeof(buff), in) !=NULL){
            oss << buff;
        }
        pclose(in);

    }catch(const std::exception& e){
        // Something went wrong
    }catch (const std::string& ex) {

    } catch (...) {

    }

}

// Get directory given file path
QString Files::getDirectory(QString filepath){
    int lastIndex = filepath.lastIndexOf("/");
    filepath.chop(filepath.length() - lastIndex);

    return filepath;
}

// Set current directory given most recently opened file
void Files::setCurrentDirectory(QString file){
    currentDirectory = getDirectory(file);
}

// Return current directory
QString Files::getCurrentDirectory(){
    return currentDirectory;
}
