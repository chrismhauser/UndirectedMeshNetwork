#include "tree.h"

tree::tree()
{
    head = new node;
    // Using a vector for memory allocation, because lazy
    ptrVect.push_back(head);
}

void tree::setHead(Ip ip)
{
    head->address = ip;
    head->weight = 0;
}

Ip tree::getHead()
{
    return head->address;
}

tree::node* tree::findNode(Ip ip)
{
    // TEST STATEMENT
    std::cout << "findNode()" << std::endl;

    // return the head if it's the ip your looking for
    if (head->address == ip)
        return head;

    node* parent = head;
    return findNode(ip, parent);
}

// Find function for recursive treversal
tree::node* tree::findNode(Ip ip, node* parent)
{
    for (size_t i = 0; i < parent->children.size(); ++i) {

        if (parent->children.at(i)->address == ip)
            return parent->children.at(i);

        node* temp = findNode(ip, parent->children.at(i));
        if(temp != nullptr) {
            return temp;
        }
    }
    // if not found return nullptr
    return nullptr;
}

std::queue<Ip> tree::findPath(Ip dest)
{
    // TEST STATEMENT
    std::cout << "findPath()" << std::endl;

    node* parent = head;
    std::stack<Ip> s;
    s.push(parent->address);
    s = findPath(dest, parent, s);

    std::stack<Ip> buffer;
    std::queue<Ip> path_q;
    Ip temp;

    while(!s.empty())
    {
        temp = s.top();
        s.pop();
        buffer.push(temp);
    }

    while(!buffer.empty())
    {
        temp = buffer.top();
        buffer.pop();
        path_q.push(temp);
    }

    std::queue<Ip> qTemp = path_q;

    return path_q;
}


// Find function for recursive treversal
std::stack<Ip> tree::findPath(Ip dest, node* parent, std::stack<Ip> s)
{
    for (size_t i = 0; i < parent->children.size(); ++i) {
        s.push(parent->children.at(i)->address);

        if (parent->children.at(i)->address == dest) {
            //s.push(parent->children.at(i)->address);
            return s;
        }

        s = findPath(dest, parent->children.at(i), s);
        if(s.top() == dest) {
            return s;
        }
        s.pop();
    }
    // if not found return nullptr
    return s;
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
             }

             // assign new values for the moved node
             check->parent = parent;
             check->weight = newWeight;
             parent->children.push_back(check);
             return;
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
