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
        int weight;
    };

    tree();
    ~tree();
    node* findNode(Ip a);
    node* findNode(Ip a, node* parent);
    std::queue<Ip> findPath(Ip dest);
    std::stack<Ip> findPath(Ip dest, node* parent, std::stack<Ip> s);
    void insertChild(node *parent, Ip ip, short connectionWeight);
    void setHead(Ip ip);
    Ip getHead();
    void print();
    void print(node* parent);


private:
    int count;
    node* head;
    std::vector<node*> ptrVect;
};

#endif // TREE_H
