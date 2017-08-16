#ifndef SECTIONLIST_H
#define SECTIONLIST_H

#include "section.h"
#include "sectionnode.h"

class SectionList
{
public:
    SectionList();
    void insert(QString section, QStringList addresses, QStringList hex, QStringList ascii);
    bool isEmpty();
    bool containsSection(QString name);
    int getLength();
    Section getSection(QString name);
    Section getSection(int index);
    QStringList getSectionNames();
private:
    SectionNode *head;
    SectionNode *tail;
    int length;
};

#endif // SECTIONLIST_H
