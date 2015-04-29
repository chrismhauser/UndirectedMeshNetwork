#include "tree.h"

tree::tree()
{
    head = new node;
    // Using a vector for memory allocation, because lazy
    ptrVect.push_back(head);
}

tree::tree(node* h)
{
    head = h;
    ptrVect.push_back(head);
}

tree::tree(Ip ip)
{
    head = new node;
    head->address = ip;
    head->weight = 0;
    ptrVect.push_back(head);
}

tree::node* tree::findNode(Ip ip)
{
    node* parent = head;
    return findNode(ip, parent);
}

// Find function for recursive treversal
tree::node* tree::findNode(Ip ip, node* parent)
{
    for (size_t i = 0; i < parent->children.size(); ++i) {

        if (parent->children.at(i)->address == ip)
            return parent->children.at(i);

        findNode(ip, parent->children.at(i));
    }
    // if not found return nullptr
    return nullptr;
}

// Used in conjunction with findNode to push children to the appropriate parent
void tree::insertChild(node *parent, Ip ip, short connectionWeight)
{
    short newWeight = parent->weight + connectionWeight;

    // first see if the node you want to add is already in the tree
    node* check = findNode(ip);
    if (check != nullptr) {

        // return if ip exists and its weight is less than the new path
        if (check->weight <= newWeight)
            return;

        // if it's weight is larger, then move it around to the shorter path
        else {
            // first remove the node from it's parent's children
            // note: iterator traversal is required here for using erase() method
            auto it = check->parent->children.begin();
            for (; it != check->parent->children.end(); it++) {

                if ((*it)->address == ip)
                   check->parent->children.erase(it);

                // assign new values for the moved node
                check->parent = parent;
                check->weight = newWeight;
                parent->children.push_back(check);
                return;
            }
        }
    }

    // if it's not already in the tree, then add it
    node* child = new node;
    child->address = ip;
    child->parent = parent;
    child->weight = newWeight;
    parent->children.push_back(child);
    ptrVect.push_back(child);
}

tree::~tree()
{
    for (size_t i = 0; i < ptrVect.size(); ++i)
    {
         delete ptrVect.at(i);
    }
}
