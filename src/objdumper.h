#ifndef OBJDUMPER_H
#define OBJDUMPER_H

#include "dataStructures/functionlist.h"
#include "dataStructures/sectionlist.h"
#include "QRegularExpression"


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
    FunctionList getFunctionList(QString file, QVector<QString> baseOffsets);
    SectionList getSectionList(QString file);
    QVector<QString> getBaseOffset(QString file);
    QVector<QString> getFileOffset(QString targetAddress, QVector<QString> baseOffsets);
    QString checkForErrors(QString file);

    void setUseCustomBinary(bool useCustom);
    void setobjdumpBinary(QString binary);
    void setOutputSyntax(QString syntax);
    void setDisassemblyFlag(QString flag);
    void setHeaderFlags(QString flags);
    void setOptionalFlags(QString);
    void setTarget(QString trgt);

private:
    QString getDump(QString args, QString file);
    QString removeHeading(QString dump, int numLines);
    QString getHeading(QString dump, int numLines);
    QString parseDumpForErrors(QString dump);

    bool useCustomBinary;
    QString objdumpBinary;
    QString outputSyntax;
    QString disassemblyFlag;
    QString headerFlags;
    QString optionalFlags;
    QString target;
    int insnwidth;
    QRegularExpression addressRegex;

};

#endif // OBJDUMPER_H
