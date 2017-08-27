#ifndef FUNCTIONLIST_H
#define FUNCTIONLIST_H

#include "functionnode.h"

class FunctionList
{
public:
    FunctionList();
    void insert(QString name, QString address, QString section, QString fileOffset, QVector< QVector<QByteArray> > contents);
    void setErrorMsg(QString msg);
    void nukeList();
    int getLength();
    bool isEmpty();
    bool containsFunction(QString name);
    bool successfullyCreated();
    Function getFunction(QString name);
    Function getFunction(int index);
    int getFunctionIndex(QString functionName);
    QStringList getFunctionNames();
    QString getErrorMsg();
    QVector<int> getAddressLocation(QString targetAddress);
    QVector< QVector<QString> > findCallsToFunction(QString targetFunction);
private:
    FunctionNode *head;
    FunctionNode *tail;
    int length;
    QString errorMsg;
};

#endif // FUNCTIONLIST_H
