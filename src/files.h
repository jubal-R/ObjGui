#ifndef READER_H
#define READER_H
#include <string>
#include "QString"

class Files
{
public:
    Files();
    QString read(QString file);
    bool write(QString fileName, QString content);
    QString getHomeDir();
    QString getDirectory(QString file);
    QString getCurrentDirectory();
    void setCurrentDirectory(QString file);
    void openFileManager(QString dir);
private:
    QString currentDirectory;
};

#endif // READER_H
