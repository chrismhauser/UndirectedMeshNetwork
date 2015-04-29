#include "node.h"

Node::Node()
{
}

void Node::forwardPacket(packet* message) {
    // if timeout is passed
    time_t now;
    time(&now);
    if (difftime(now, message->timout) >= 0) {
        emit packetDiscarded();
    } else if (message->reciever->address == this->address) {
        if(message->data == "Ack") {
            message->sender->packetIndex++;
        } else {
            emit packetRecieved();
        }
    } else {
        message->path.pop();

        //


        for(int i=0; i<connectedNodes.size(); i++) {
            if(connectedNodes.at(i)->address == message->path.front()) {
                connectedNodes.at(i)->forwardPacket(message);
                break;
            }
        }
    }

}
