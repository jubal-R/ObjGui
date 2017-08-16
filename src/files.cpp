#include "files.h"
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

Files::Files()
{
    currentDirectory = getHomeDir();
}

//  Read File And Return It's Contents
QString Files::read(QString file){
    string fileStr = file.toStdString();
    string line = "";
    string content = "";
    ifstream fileReader;

    try{
    fileReader.open(fileStr.c_str());

    while(fileReader.good()){
        getline(fileReader, line);
        content += line+"\n";
        }
    fileReader.close();

    }catch(exception e){
    }

    if(content.length() > 1){
        content.resize(content.length() - 1);
    }

    return QString::fromStdString(content);
}

//  Write String Content To File
bool Files::write(QString filepath, QString contents){
    string fileName = filepath.toStdString();
    string content = contents.toStdString();
    try{
        ofstream file;
        file.open(fileName.c_str());
        file << content;
        file.close();
        return true;
    }catch(exception e){
        return false;
    }
}

QString Files::getCurrentDirectory(){
    return currentDirectory;
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

//  Return Users Home Directory
QString Files::getHomeDir(){
    ostringstream oss;
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

void Files::openFileManager(QString dir){
    // xdg-open + dir
    ostringstream oss;
    FILE *in;
    char buff[100];
    string cmd = "xdg-open " + dir.toStdString() + " 2>&1";

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
