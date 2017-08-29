#ifndef READER_H
#define READER_H

#include "QString"

class Files
{
public:
    Files();
    QVector< QVector<QByteArray> > strings(QString file);
    bool isPrintableChar(char c);
    QString getHomeDir();
    QString getDirectory(QString file);
    QString getCurrentDirectory();
    void setCurrentDirectory(QString file);
    void openFileManager(QString dir);
private:
    QString currentDirectory;
};

#endif // READER_H
