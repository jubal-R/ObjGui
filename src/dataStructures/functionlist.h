#ifndef FUNCTIONLIST_H
#define FUNCTIONLIST_H

#include "functionnode.h"

class FunctionList
{
public:
    FunctionList();
    void insert(QString name, QString address, QString section, QString fileOffset, QVector< QVector<QByteArray> > contents);
    void nukeList();
    int getLength();
    bool isEmpty();
    bool containsFunction(QString name);
    Function getFunction(QString name);
    Function getFunction(int index);
    int getFunctionIndex(QString functionName);
    QStringList getFunctionNames();
    QVector<int> getAddressLocation(QString targetAddress);
    QVector< QVector<QString> > findCallsToFunction(QString targetFunction);
    QVector< QVector<QString> > findCallsToAddress(QString targetAddress);
private:
    FunctionNode *head;
    FunctionNode *tail;
    int length;
};

#endif // FUNCTIONLIST_H
