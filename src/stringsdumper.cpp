#include "stringsdumper.h"

#include "QFile"
#include "QVector"

StringsDumper::StringsDumper()
{

}

// Extract strings from file along with their file offset and vma
QVector< QVector<QString> > StringsDumper::dumpStrings(QString filename, QVector<QString> baseOffsets){
    QVector< QVector<QString> > stringsData;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return stringsData;

    // Seek to start of first section
    bool ok;
    qint64 startPos = baseOffsets[1].toLongLong(&ok, 16);
    if (ok)
        file.seek(startPos);

    // Read each byte
    while (!file.atEnd()) {
        QByteArray bytes = file.read(1);
        char byte = bytes[0];

        // If read byte is a printable character start builing string
        if (isPrintableChar(byte)){
            qint64 pos = file.pos();
            QString str = bytes;

            // Build string until running into a nonprintable character
            while (!file.atEnd()){
                bytes = file.read(1);
                byte = bytes[0];
                if (isPrintableChar(byte)){
                    str.append(byte);
                } else {
                    break;
                }
            }

            // Add built string to results if it meets length requirement
            if(str.length() >= 4){
                QVector<QString> stringData(2);
                stringData[0] = "0x" + getAddressFromOffset(QString::number(pos - 1, 16), baseOffsets);
                stringData[1] = str;

                stringsData.append(stringData);
            }
        }
    }

    return stringsData;
}

bool StringsDumper::isPrintableChar(char c){
    if ((c >= 32 && c <= 126) || c == 9 || c== 10)
        return true;
    else
        return false;
}

// Return virtual memory address of file offset given base offsets
QString StringsDumper::getAddressFromOffset(QString offset, QVector<QString> baseOffsets){
    QString address = "";
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
