#include "sectionlist.h"

SectionList::SectionList()
{
    head = NULL;
    tail = NULL;
    length = 0;
}

void SectionList::insert(QString section, QVector< QVector<QByteArray> > contents){
    SectionNode *newNode = new SectionNode(section, contents);

    if (isEmpty()){
        head = newNode;
        tail = head;
    } else {
        tail->setNext(newNode);
        tail = tail->getNext();
    }
    length++;
}

void SectionList::nukeList(){
    if (!isEmpty()){
        SectionNode *current = head;
        SectionNode *prev = head;

        while(current->getNext() != NULL){
            current = current->getNext();
            delete prev;
            prev = current;
        }
        delete current;

        head = NULL;
        tail = NULL;
        length = 0;

    }
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
    // If not found return empty section
    QVector< QVector<QByteArray> > empty;
    Section s("", empty);
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
    // If not found return empty section
    QVector< QVector<QByteArray> > empty;
    Section s("", empty);
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

