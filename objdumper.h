#ifndef OBJDUMPER_H
#define OBJDUMPER_H

#include "QString"


class ObjDumper
{
public:
    ObjDumper(QString File);
    QString getDisassembly();
    QString getSymbolsTable();
    QString getRelocationEntries();
    QString getStrings();
    QStringList getFunctionsList();
    QList<int> getSectionIndices();

private:
    QString getDump(QString args, QString file);
    void setDisassembly(QString file);
    void setSymbolsTable(QString file);
    void setRelocationEntries(QString file);
    void setStrings(QString file);
    void setFunctionsLists(QString disassembly);

};

#endif // OBJDUMPER_H
