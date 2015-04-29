#ifndef TREE_H
#define TREE_H
#include <vector>
#include <queue>
#include <stack>
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
    std::queue<Ip> findPath(Ip dest);
    std::stack<Ip> findPath(Ip dest, node* parent, std::stack<Ip> s);
    void insertChild(node *parent, Ip ip, short connectionWeight);

private:
    node* head;
    std::vector<node*> ptrVect;
};

#endif // TREE_H
