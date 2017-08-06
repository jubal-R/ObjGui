#ifndef OBJDUMPER_H
#define OBJDUMPER_H

#include "QString"
#include "QStringList"
#include "QList"


class ObjDumper
{
public:
    ObjDumper(QString File, QString syntax);
    QString getDisassembly();
    QString getSymbolsTable();
    QString getRelocationEntries();
    QString getStrings();
    QString getHeaders();
    QStringList getFunctionsList();
    QList<int> getSectionIndices();

private:
    QString getDump(QString args, QString file);
    void setDisassembly(QString file);
    void setSymbolsTable(QString file);
    void setRelocationEntries(QString file);
    void setStrings(QString file);
    void setHeaders(QString file);
    void setFunctionsLists(QString disassembly);
    QList<int> sectionIndices;
    QStringList funtionsList;
    QString disassembly;
    QString symbolsTable;
    QString relocationEntries;
    QString strings;
    QString headers;
    QString outputSyntax;

};

#endif // OBJDUMPER_H
