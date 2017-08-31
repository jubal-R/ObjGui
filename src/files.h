#ifndef READER_H
#define READER_H

#include "QString"

class Files
{
public:
    Files();
    QVector< QVector<QString> > strings(QString file, QVector<QString> baseOffsets);
    bool isPrintableChar(char c);
    QString getAddressFromOffset(QString offset, QVector<QString> baseOffsets);
    QString getDirectory(QString file);
    QString getCurrentDirectory();
    void setCurrentDirectory(QString file);
    void openFileManager(QString dir);
private:
    QString currentDirectory;
};

#endif // READER_H
