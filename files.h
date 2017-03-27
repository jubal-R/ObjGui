#ifndef READER_H
#define READER_H
#include <string>

class Files
{
public:
    Files();
    std::string read(std::string file);
    bool write(std::string fileName, std::string content);
    std::string getHomeDir();
};

#endif // READER_H
