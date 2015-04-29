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
    // TEST STATEMENT
    std::cout << "findNode()" << std::endl;

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

tree::~tree()
{
    for (size_t i = 0; i < ptrVect.size(); ++i)
    {
         delete ptrVect.at(i);
    }
}
