#ifndef READER_H
#define READER_H

#include "QString"

class Files
{
public:
    Files();

    QString getDirectory(QString file);
    QString getCurrentDirectory();
    void setCurrentDirectory(QString file);
private:
    QString currentDirectory;
};

#endif // READER_H
