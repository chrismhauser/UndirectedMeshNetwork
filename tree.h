#ifndef TREE_H
#define TREE_H
#include <vector>
#include "ip.h"
#include "iostream"

class tree
{
public:
    struct node {
        node* parent;
        Ip address;
        std::vector<node*> children;
        short weight;
    };

    tree();
    tree(node *h);
    tree(Ip ip);
    ~tree();
    node* findNode(Ip a);
    node* findNode(Ip a, node* parent);
    void insertChild(node *parent, Ip ip, short connectionWeight);

private:
    node* head;
    std::vector<node*> ptrVect;
};

#endif // TREE_H
