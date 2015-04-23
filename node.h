#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <QObject>

class Node
{
public:
    Node();

    struct packet {
        Node *sender, *reciever;
        int packetId;
        std::string data;
        // TODO time timeout
    };

    std::vector<Node*> connectedNodes;
    Ip address;
    // TODO std::vector<Queue> previousPaths;

    void forwardPacket(packet message) { // TODO add path parameter
        // TODO check packet timeout (discard if past, else forward)
        if (1) { // TODO if timeout is passed
            emit packetDiscarded();
        } else if (message.reciever->address == this->address) {
            emit packetRecieved();
        } else {
            forwardPacket(message); // TODO add path
        }

    }

signals:
    void packetDiscarded(); // TODO add path parameter
    void packetRecieved(); // TODO add path parameter
};

#endif // NODE_H


struct node {

};
