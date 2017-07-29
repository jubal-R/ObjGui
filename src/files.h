#ifndef READER_H
#define READER_H
#include <string>
#include "QString"

class Files
{
public:
    Files();
    std::string read(std::string file);
    bool write(std::string fileName, std::string content);
    std::string getHomeDir();
    void openFileManager(QString dir);
};

#endif // READER_H
