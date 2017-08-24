#include "sectionnode.h"

SectionNode::SectionNode(QString sectionName, QVector< QVector<QByteArray> > contents)
    : section(sectionName, contents)
{
    next = NULL;
}

// Set Next Node
void SectionNode::setNext(SectionNode *n){
    next = n;
}

// Get Next Node
SectionNode* SectionNode::getNext(){
    return next;
}

// Get Section Object

Section SectionNode::getSection(){
    return section;
}
