//Copyright (C) 2016  Jubal
#include "files.h"
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

Files::Files()
{
}

//  Read File And Return It's Contents
string Files::read(string file){
    string line = "";
    string content = "";
    ifstream fileReader;

    try{
    fileReader.open(file.c_str());

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

    return content;
}

//  Write String Content To File
bool Files::write(string fileName, string content){
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

//  Return Users Home Directory
string Files::getHomeDir(){
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

    return oss.str().substr(0, oss.str().length()-1);
}
