#include "stringsdumper.h"

#include "QFile"
#include "QVector"

#include <QDebug>

// Extract strings from file along with their file offset and vma
QVector< QVector<QString> > StringsDumper::dumpStrings(QString filename, const QVector<QString> &baseOffsets){
    QVector< QVector<QString> > stringsData;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return stringsData;

    // Seek to start of first section
    bool ok{};
    const qint64 startAddr = baseOffsets[0].toLongLong(&ok, 16);
    qint64 startPos = baseOffsets[1].toLongLong(&ok, 16);
    if (!ok)
        startPos = 0;

    const auto bytes = file.readAll();
    for (int i = startPos; i < bytes.length(); ++i) {
        char c = bytes[i];

        if (!isprint(c))
            continue;

        int pos = i;
        QString str(c);

        for (int j = pos; j < bytes.length(); j++) {
            if (isprint(bytes[j])) {
                char b = bytes[j];
                str.append(b);
                i++;
            } else {
                i = j;
                break;
            }
        }

        if(str.length() >= 4){
            QVector<QString> stringData(2);
            stringData[0] = QStringLiteral("0x") + getAddressFromOffset(pos - 1, startAddr, startPos);
            stringData[1] = str;

            stringsData.append(stringData);
        }
    }

    return stringsData;
}

// Return virtual memory address of file offset given base offsets
QString StringsDumper::getAddressFromOffset(qint64 offset, qint64 baseAddr, qint64 baseOffset){

    qlonglong offsetFromBase = offset - baseOffset;
    if (offsetFromBase >= 0) {
        qlonglong targetAddress = baseAddr + offsetFromBase;
        return QString::number(targetAddress, 16);
    }

    return QString();
}
