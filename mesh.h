#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <algorithm>
#include <iterator>

#include "ip.h"
#include "node.h"

#define DS_ID_GRID 0
#define DS_ID_MAP 1

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

class Mesh : public QObject
{
    Q_OBJECT
private:
    Node** nodeGrid;
    int gridSize;

    std::unordered_map<Ip, Node> nodeMap;
    int mapSize;

    void generateGrid(size_t size = 256);
    void generateMap(size_t size = 75);

    void reversePath(std::queue<Ip>& path);

    Node::packet* currentMessage;
    int CUR_DS_ID;

public:
    Mesh();
    Mesh(size_t ds_id, size_t size);

    std::queue<Ip> findPath(Node* sender, Node* reciever);
    Node::packet* generatePacket();
    void sendPacket(Node::packet* message);

public slots:
    // on signal packetDiscarded
    void resendPacket();

    // on signal packetRecieved
    void sendAck();
};

#endif // MESH_H
