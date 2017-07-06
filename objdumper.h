#ifndef OBJDUMPER_H
#define OBJDUMPER_H

#include "QString"


class ObjDumper
{
public:
    ObjDumper();
    QString getDump(QString args, QString file);
    QString getDisassembly(QString file);
    QString getSymbolsTable(QString file);
    QString getRelocationEntries(QString file);
    QString getStrings(QString file);
    QStringList getFunctionsList(QString dump);
};

#endif // OBJDUMPER_H
