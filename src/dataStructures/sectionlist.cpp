#include "sectionlist.h"

SectionList::SectionList()
{
    head = NULL;
    tail = NULL;
    length = 0;
}

void SectionList::insert(QString section, QStringList addresses, QStringList hex, QStringList ascii){
    SectionNode *newNode = new SectionNode(section, addresses, hex, ascii);

    if (isEmpty()){
        head = newNode;
        tail = head;
    } else {
        tail->setNext(newNode);
        tail = tail->getNext();
    }
    length++;
}

bool SectionList::isEmpty(){
    if (head == NULL)
        return true;
    else
        return false;
}

bool SectionList::containsSection(QString name){
    if (!isEmpty()){
        if (head->getSection().getSectionName() == name)
            return true;

        SectionNode *p = head;
        while (p->getNext() != NULL){
            p = p->getNext();
            if (p->getSection().getSectionName() == name)
                return true;
        }
    }
    return false;
}

int SectionList::getLength(){
    return length;
}

Section SectionList::getSection(QString name){
    if (!isEmpty()){
        if (head->getSection().getSectionName() == name)
            return head->getSection();

        SectionNode *p = head;
        while (p->getNext() != NULL){
            p = p->getNext();
            if (p->getSection().getSectionName() == name)
                return p->getSection();
        }
    }
    // If not found return empty function
    Section s("", {}, {}, {});
    return s;
}

Section SectionList::getSection(int index){
    if (!isEmpty() && index < length){
        SectionNode *p = head;
        for (int i = 0; i < index; i++){
            p = p->getNext();
        }
        return p->getSection();
    }
    // If not found return empty function
    Section s("", {}, {}, {});
    return s;
}

QStringList SectionList::getSectionNames(){
    QStringList sectionNames;
    if (!isEmpty()){
        sectionNames.append(head->getSection().getSectionName());
        SectionNode *p = head;
        while (p->getNext() != NULL){
            p = p->getNext();
            sectionNames.append(p->getSection().getSectionName());
        }
    }

    return sectionNames;
}


