#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>

#include <vector>

#include "ip.h"
#include "node.h"

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
    std::vector<std::vector<Node> > nodeGrid;
    std::unordered_map<Ip, Node> nodeMap;

    void generateGrid(size_t size = 256);
    void generateMap(size_t size = 75);

    Node::packet currentMessage;

    const size_t DS_ID_GRID = 0;
    const size_t DS_ID_MAP = 1;

public:
    Mesh();
    Mesh(size_t ds_id, size_t size);

    // TODO Queue findPath(Node* sender, Node* reciever);
    Node::packet generatePacket();
    void sendPacket(packet message);

public slots:
    // on signal packetDiscarded
    void resendPacket();

    // on signal packetRecieved
    void sendAck(Node* sender, Node* reciever, int packetId);
};

#endif // MESH_H
