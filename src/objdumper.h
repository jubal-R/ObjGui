#ifndef OBJDUMPER_H
#define OBJDUMPER_H

#include "dataStructures/functionlist.h"
#include "dataStructures/sectionlist.h"
#include "QRegularExpression"


class ObjDumper
{
public:
    ObjDumper();
    FunctionList getFunctionList(QString file, QVector<QString> baseOffsets);
    SectionList getSectionList(QString file);
    QString getSymbolsTable(QString file);
    QString getRelocationEntries(QString file);
    QString getHeaders(QString file);
    QString getFileFormat(QString file);
    QVector<QString> getBaseOffset(QString file);
    QString checkForErrors(QString file);
    static QVector<QString> getFileOffset(QString targetAddress, QVector<QString> baseOffsets);

    void setUseCustomBinary(bool useCustom);
    void setobjdumpBinary(QString binary);
    void setOutputSyntax(QString syntax);
    void setDisassemblyFlag(QString flag);
    void setDemangleFlag(QString flag);
    void setTarget(QString trgt);
    void setArchiveHeaderFlag(QString flag);
    void setFileHeaderFlag(QString flag);
    void setPrivateHeaderFlag(QString flag);
    void setSectionsHeaderFlag(QString flag);

private:
    QString getDump(QStringList argsList);
    QString getDisassembly(QString file);
    QString getContents(QString file);
    QString removeHeading(QString dump, int numLines);
    QString getHeading(QString dump, int numLines);
    QString parseDumpForErrors(QString dump);
    QVector<QByteArray> parseFunctionLine(QStringRef line);
    QVector<QByteArray> parseSectionLine(QStringRef line);

    bool useCustomBinary;
    QString objdumpBinary;
    QString outputSyntax;
    QString disassemblyFlag;
    QString archiveHeaderFlag;
    QString fileHeaderFlag;
    QString privateHeaderFlag;
    QString sectionsHeaderFlag;
    QString demangleFlag;
    QString target;
    int insnwidth;
    QRegularExpression addressRegex;

};

#endif // OBJDUMPER_H
