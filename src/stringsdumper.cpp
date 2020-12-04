#include "stringsdumper.h"

#include "QFile"
#include "QVector"

// Extract strings from file along with their file offset and vma
QVector< QVector<QString> > StringsDumper::dumpStrings(QString filename, const QVector<QString> &baseOffsets){
    QVector< QVector<QString> > stringsData;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return stringsData;

    // Seek to start of first section
    bool ok;
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
            stringData[0] = QStringLiteral("0x") + getAddressFromOffset(QString::number(pos - 1, 16), baseOffsets);
            stringData[1] = str;

            stringsData.append(stringData);
        }
    }

    return stringsData;
}

// Return virtual memory address of file offset given base offsets
QString StringsDumper::getAddressFromOffset(QString offset, const QVector<QString> &baseOffsets){
    QString address;
    bool targetOffsetOk;
    bool baseAddrOk;
    bool baseOffsetOk;
    qlonglong targetOffset = offset.toLongLong(&targetOffsetOk, 16);
    qlonglong baseAddr = baseOffsets[0].toLongLong(&baseAddrOk, 16);
    qlonglong baseOffset = baseOffsets[1].toLongLong(&baseOffsetOk, 16);

    if(targetOffsetOk && baseAddrOk && baseOffsetOk){
        qlonglong offsetFromBase = targetOffset - baseOffset;
        if(offsetFromBase >= 0){
            qlonglong targetAddress = baseAddr + offsetFromBase;
            address = QString::number(targetAddress, 16);
        }
    }

    return address;
}
