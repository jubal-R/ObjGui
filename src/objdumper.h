#ifndef OBJDUMPER_H
#define OBJDUMPER_H

#include "QString"
#include "QStringList"
#include "QList"
#include "functionlist.h"


class ObjDumper
{
public:
    ObjDumper();
    QString getDisassembly(QString file);
    QString getSymbolsTable(QString file);
    QString getRelocationEntries(QString file);
    QString getContents(QString file);
    QString getHeaders(QString file);
    QString getFileFormat(QString file);
    FunctionList getFunctionList(QString file);

private:
    QString getDump(QString args, QString file);
    QString removeHeading(QString dump);

    QString outputSyntax;

};

#endif // OBJDUMPER_H
