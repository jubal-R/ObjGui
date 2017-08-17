#ifndef OBJDUMPER_H
#define OBJDUMPER_H

#include "QString"
#include "QStringList"
#include "QList"
#include "dataStructures/functionlist.h"
#include "dataStructures/sectionlist.h"


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
    SectionList getSectionList(QString file);

    void setobjdumpBinary(QString binary);
    void setOutputSyntax(QString syntax);
    void setDisassemblyFlag(QString flag);
    void setHeaderFlags(QString flags);
    void setOptionalFlags(QString);

private:
    QString getDump(QString args, QString file);
    QString removeHeading(QString dump, int numLines);

    QString objdumpBinary;
    QString outputSyntax;
    QString disassemblyFlag;
    QString headerFlags;
    QString optionalFlags;

};

#endif // OBJDUMPER_H
