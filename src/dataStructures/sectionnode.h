#ifndef SECTIONNODE_H
#define SECTIONNODE_H

#include "section.h"

class SectionNode
{
public:
    SectionNode(QString sectionName, QVector< QVector<QByteArray> > contents);
    void setNext(SectionNode *n);
    SectionNode* getNext();
    Section getSection();
private:
    SectionNode *next;
    Section section;
};

#endif // SECTIONNODE_H
