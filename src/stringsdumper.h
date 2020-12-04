#ifndef STRINGSDUMPER_H
#define STRINGSDUMPER_H

#include "QString"

class StringsDumper
{
public:
    StringsDumper() = default;
    QVector< QVector<QString> > dumpStrings(QString file, const QVector<QString>& baseOffsets);
private:
    QString getAddressFromOffset(QString offset, const QVector<QString>& baseOffsets);
};

#endif // STRINGSDUMPER_H
