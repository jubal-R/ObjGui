#ifndef OBJDUMPER_H
#define OBJDUMPER_H

#include "QRegularExpression"

#include "model/function.h"
#include "model/section.h"


class ObjDumper
{
public:
    ObjDumper();
    QVector<Function> getFunctionData(QString file, QVector<QString> baseOffsets);
    QVector<Section> getSectionData(QString file);
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

private:
    QString getDump(QStringList argsList);
    QString getDisassembly(QString file);
    QString getContents(QString file);
    QString removeHeading(QString dump, int numLines);
    QString getHeading(QString dump, int numLines);
    QByteArray parseAddress(QByteArray address);
    QByteArray parseHexBytes(QByteArray byteString);
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
    QRegularExpression hexBytesRegex;

};

#endif // OBJDUMPER_H
