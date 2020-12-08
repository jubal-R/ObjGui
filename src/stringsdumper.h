#ifndef STRINGSDUMPER_H
#define STRINGSDUMPER_H

#include "QString"

class StringsDumper
{
public:
    StringsDumper() = default;
    QVector< QVector<QString> > dumpStrings(QString file, const QVector<QString>& baseOffsets);
private:
    QString getAddressFromOffset(qint64 offset, qint64 baseAddr, qint64 baseOffset);
};

#endif // STRINGSDUMPER_H
