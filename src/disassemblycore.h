#ifndef DISASSEMBLYCORE_H
#define DISASSEMBLYCORE_H

#include "QString"

#include "model/function.h"
#include "model/section.h"
#include "model/strings.h"

#include "objdumper.h"
#include "stringsdumper.h"

class DisassemblyCore
{
public:
    DisassemblyCore();
    void disassemble(QString file);
    bool disassemblyIsLoaded();
    void xrefStrings();
    static QString extractAddress(const QByteArray& s);
    QString getFileName();
    QVector<QString> getBaseOffsets();
    QString getObjdumpErrorMsg(QString file);
    QString getSymbolsTable(QString file);
    QString getRelocationEntries(QString file);
    QString getHeaders(QString file);
    QString getFileFormat(QString file);
    QVector<QString> getFileOffset(QString targetAddress);
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

    QStringList getFunctionNames();
    Function getFunction(QString name);
    Function getFunction(int index);
    int getFunctionIndex(QString functionName);
    bool functionExists(QString name);
    QVector<int> getAddressLocation(QString targetAddress);
    QVector< QVector<QString> > findCallsToFunction(QString targetFunction);
    QVector< QVector<QString> > findReferences(QString target);

    QString getSectionHexDump();
    QString getSectionAddressDump();

    QString getStringsAddresses();
    QString getStrings();
    int getStringIndexByAddress(QString address);
    QString getStringAddressAt(int index);

private:
    QVector<Function> functionData;
    QVector<Section> sectionData;
    Strings strings;
    QVector<QString> baseOffsets;
    bool fileLoaded;
    QString fileName;


    ObjDumper objDumper;
    StringsDumper stringsDumper;
};

#endif // DISASSEMBLYCORE_H
