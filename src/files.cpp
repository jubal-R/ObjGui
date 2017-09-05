#include "files.h"

#include "QDir"

#include "QDebug"

Files::Files()
{
    // Users home directory by default
    currentDirectory = QDir::homePath();
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
