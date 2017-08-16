#include "section.h"


Section::Section()
{

}

Section::Section(QString section, QStringList addresses, QStringList hex, QStringList ascii){
    sectionName = section;
    addressList = addresses;
    hexList = hex;
    asciiList = ascii;
}

QString Section::getSectionName(){
    return sectionName;
}

QStringList Section::getAddressList(){
    return addressList;
}

QStringList Section::getHexList(){
    return hexList;
}

QStringList Section::getAsciiList(){
    return asciiList;
}
