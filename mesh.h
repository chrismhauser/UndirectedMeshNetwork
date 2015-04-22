#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>

#include <vector>

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
        // TODO std::vector<Queue> previousPaths;

        void forwardPacket(packet message) { // TODO add path parameter
            // TODO check packet timeout (discard if past, else forward)
            if (1) // TODO if timeout is passed
                emit packetDiscarded();
            else if (message.reciever.address == this->address)
                emit packetRecieved();
            else
                forwardPacket(message); // TODO add path

        }

    signals:
        void packetDiscarded(); // TODO add path parameter
        void packetRecieved(); // TODO add path parameter
    };

    struct packet {
        node* sender, reciever;
        int packetId;
        std::string data;
        // TODO time timeout
    };

    std::vector<std::vector<node>> nodeGrid;
    std::unordered_map<Ip, node> nodeMap;

    void generateGrid(size_t size = 256);
    void generateMap(size_t size = 75);

    packet currentMessage;

    size_t DS_ID_GRID = 0;
    size_t DS_ID_MAP = 1;

public:
    Mesh();
    Mesh(size_t ds_id, size_t size);

    // TODO Queue findPath(node* sender, node* reciever);
    packet generatePacket();
    void sendPacket(packet message);

public slots:
    // on signal packetDiscarded
    void resendPacket();

    // on signal packetRecieved
    void sendAck(node* sender, node* reciever, int packetId);
};

#endif // MESH_H
