#include "sectionnode.h"

SectionNode::SectionNode(QString sectionName, QStringList addresses, QStringList hex, QStringList ascii)
    : section(sectionName, addresses, hex, ascii)
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
