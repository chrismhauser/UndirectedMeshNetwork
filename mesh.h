#ifndef MESH_H
#define MESH_H

#include <vector>

#include "ip.h"

class Mesh
{

private:
    struct node {
        std::vector<node*> connectedNodes;
        Ip address;
    };

    struct packet {
        node* sender, reciever;
        std::string data;
    };

    // For grid - 2D array of nodes
    std::vector<node> nodeGrid[][];
    // For mesh - hash table

public:
    Mesh();

    void send(node* sender, node* reciever, packet message);

    // On send - emit packet sent
    // On recieve - respond with ack
};

#endif // MESH_H
