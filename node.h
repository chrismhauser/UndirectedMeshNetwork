#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <QObject>
#include <queue>
#include <unordered_map>
#include <time.h>

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

class Node : public QObject
{
    Q_OBJECT
public:
    Node();

    struct packet {
        Node *sender, *reciever, *interm;
        int packetId = 0;
        std::string data;
        time_t timout, sendTime;
        std::queue<Ip> path;
    };

    std::vector<Node*> connectedNodes;
    std::vector<int> connectorWeights;
    Ip address;
    int packetIndex;

    // TODO: routing table (probably hash table or stack)
    // I was thinking mapping destination Ip to the integer indices of the
    // connectedNodes vector:
    std::unordered_map<Ip,short> routingTable;


    void forwardPacket(packet* message);

signals:
    void packetDiscarded();
    void packetRecieved();
};

#endif // NODE_H
