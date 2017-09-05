#ifndef STRINGSDUMPER_H
#define STRINGSDUMPER_H

#include "QString"

class StringsDumper
{
public:
    StringsDumper();
    QVector< QVector<QString> > dumpStrings(QString file, QVector<QString> baseOffsets);
private:
    bool isPrintableChar(char c);
    QString getAddressFromOffset(QString offset, QVector<QString> baseOffsets);
};

#endif // STRINGSDUMPER_H
