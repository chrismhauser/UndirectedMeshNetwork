
#include "mesh.h"

Mesh::Mesh(){}

Mesh::Mesh(size_t ds_id, size_t size)
{
    switch(ds_id)
    {
    case DS_ID_GRID:
        generateGrid(size);
        break;
    case DS_ID_MAP:
        generateMap(size);
    }
}

void Mesh::generateGrid(int size = 256)
{
    /* TODO generate grid of nodes (default size = 256)
     *  assign Ip addresses
     *  connect node signals to mesh slots
     */
}

void Mesh::generateMap(int size = 75)
{
    /* TODO generate map of nodes (default size = 75)
     *  assign Ip addresses
     *  connect node signals to mesh slots
     */
}

Mesh::packet Mesh::generatePacket()
{
    /* TODO
    *   choose random sender and reciever
    *   set packetId
    *   set random data (for loop random # of times, str += "data")
    */
}

void Mesh::sendPacket(packet message)
{
    currentMessage = message;
    // TODO if new path, have sender save path
    // TODO set message timeout timer
    message.sender->forwardPacket(message);
}

void Mesh::resendPacket()
{
    sendPacket(currentMessage);
}

void Mesh::sendAck(node *sender, node *reciever, int packetId)
{
    packet* ack = new ack;
    ack->sender = reciever;
    ack->reciever = sender;
    ack->packetId = packetId;
    ack->data = "Ack";
    // TODO ack->timeout = timeout

}


