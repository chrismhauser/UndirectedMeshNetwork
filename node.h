#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <QObject>
#include <queue>
#include <time.h>

#include "ip.h"

class Node
{
public:
    Node();

    struct packet {
        Node *sender, *reciever;
        int packetId;
        std::string data;
        time_t timout;
        std::queue<Ip> path;
    };

    std::vector<Node*> connectedNodes;
    Ip address;
    int packetIndex;
    // TODO routing table (probably hash table or stack)

    void forwardPacket(packet* message);

signals:
    void packetDiscarded(packet* message);
    void packetRecieved(packet* message);
};

#endif // NODE_H
