#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>

#include "ip.h"

// Struct for adding specialization to std::hash, which doesn't support hashing
// the object Ip.
namespace std
{
    template <>
    struct hash<Ip>
    {
        size_t operator()(const Ip& ip) const
        {
            return (hash<int>()(ip.getIpNum(3)) ^ (hash<int>()(ip.getIpNum(4) << 1)) >> 1);
        }
    };
}

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

    std::vector<std::vector<node>> nodeGrid;
    std::unordered_map<Ip, node> nodeMap;

public:
    Mesh();

    void send(node* sender, node* reciever, packet message);

    // On send - emit packet sent
    // On recieve - respond with ack
};

#endif // MESH_H
