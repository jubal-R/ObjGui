#include "files.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include "QVector"
#include "QFile"

#include "QDebug"

Files::Files()
{
    currentDirectory = getHomeDir();
}

// Extract strings from file along with their file offset and vma
QVector< QVector<QString> > Files::strings(QString filename, QVector<QString> baseOffsets){
    QVector< QVector<QString> > stringsData;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return stringsData;

    // Seek to start of first section
    bool ok;
    qint64 startPos = baseOffsets[1].toLongLong(&ok, 16);
    if (ok)
        file.seek(startPos);

    // Read each byte
    while (!file.atEnd()) {
        QByteArray bytes = file.read(1);
        char byte = bytes[0];

        // If read byte is a printable character start builing string
        if (isPrintableChar(byte)){
            qint64 pos = file.pos();
            QString str = bytes;

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
                QVector<QString> stringData(2);
                stringData[0] = getAddressFromOffset(QString::number(pos - 1, 16), baseOffsets);
                stringData[1] = str;

                stringsData.append(stringData);
            }
        }
    }

    return stringsData;
}

bool Files::isPrintableChar(char c){
    if ((c >= 32 && c <= 126) || c == 9 || c== 10)
        return true;
    else
        return false;
}

// Return virtual memory address of file offset given base offsets
QString Files::getAddressFromOffset(QString offset, QVector<QString> baseOffsets){
    QString address = "";
    bool targetOffsetOk;
    bool baseAddrOk;
    bool baseOffsetOk;
    qlonglong targetOffset = offset.toLongLong(&targetOffsetOk, 16);
    qlonglong baseAddr = baseOffsets[0].toLongLong(&baseAddrOk, 16);
    qlonglong baseOffset = baseOffsets[1].toLongLong(&baseOffsetOk, 16);

    if(targetOffsetOk && baseAddrOk && baseOffsetOk){
        qlonglong offsetFromBase = targetOffset - baseOffset;
        if(offsetFromBase >= 0){
            qlonglong targetAddress = baseAddr + offsetFromBase;
            address = QString::number(targetAddress, 16);
        }
    }

    return address;
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

    return QString::fromStdString(oss.str());
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

// Get directory given file path(example: /bin/bash returns /bin/)
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
