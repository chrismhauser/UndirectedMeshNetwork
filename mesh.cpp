
#include "mesh.h"

Mesh::Mesh(){
    Mesh(0,256);
}

Mesh::Mesh(size_t ds_id, size_t size)
{
    switch(ds_id)
    {
    case DS_ID_GRID:
        CUR_DS_ID = DS_ID_GRID;
        generateGrid(size);
        break;
    case DS_ID_MAP:
        CUR_DS_ID = DS_ID_MAP;
        generateMap(size);
        break;
    default:
        generateGrid(size);
        break;
    }

    currentMessage = generatePacket();
    sendPacket(currentMessage);
}

void Mesh::generateGrid(size_t size /*= 256*/)
{
    /*  generate grid of nodes (default size = 256)
     *  assign Ip addresses
     *  connect node signals to mesh slots
     */

    gridSize = size;
    nodeGrid = new Node*[size];
    for(int i=0; i<size; i++) {
        nodeGrid[i] = new Node[size];
        for(int j=0; j<size; j++) {
            nodeGrid[i][j].address.setIp(0,0,i,j);

            // Connect neighboring nodes (top and left)
            if(i > 0) {
                // connect both ways
                nodeGrid[i][j].connectedNodes.push_back(&nodeGrid[i-1][j]);
                nodeGrid[i][j].connectorWeights.push_back(rand() % 5);

                nodeGrid[i-1][j].connectedNodes.push_back(&nodeGrid[i][j]);
                nodeGrid[i-1][j].connectorWeights.push_back(rand() % 5);
            }

            if(j > 0) {
                // connect both ways
                nodeGrid[i][j].connectedNodes.push_back(&nodeGrid[i][j-1]);
                nodeGrid[i][j].connectorWeights.push_back(rand() % 5);

                nodeGrid[i][j-1].connectedNodes.push_back(&nodeGrid[i][j]);
                nodeGrid[i][j-1].connectorWeights.push_back(rand() % 5);
            }

            // Connect node signals (packet recieved/discarded) to mesh slots (sendAck/resend)
            QObject::connect(&nodeGrid[i][j], SIGNAL(packetDiscarded()), this, SLOT(resendPacket()));
            QObject::connect(&nodeGrid[i][j], SIGNAL(packetRecieved()), this, SLOT(sendAck()));
        }
    }
}

void Mesh::generateMap(size_t size /*= 75*/)
{
    /* TODO generate map of nodes (default size = 75)
     *  assign Ip addresses
     *  connect node signals to mesh slots
     */
}

void Mesh::reversePath(std::queue<Ip>& path)
{
    std::stack<Ip> s;
    Ip temp;

    while(!path.empty())
    {
        temp = path.front();
        path.pop();
        s.push(temp);
    }

    while(!s.empty())
    {
        temp = s.top();
        s.pop();
        path.push(temp);
    }
}

Node::packet* Mesh::generatePacket()
{
    /* TODO
    *   choose random sender and reciever (grid - done, map - not done)
    *   set packetId
    *   set random data (for loop random # of times, str += "data")
    */

    Node::packet* message;

    // Choose random sender & reciever
    switch(CUR_DS_ID)
    {
    case DS_ID_GRID:
      {
        int randX1, randY1;
        int randX2, randY2;
        bool pass = false;

        // Randomize Sender
        randX1 = rand() % gridSize;
        randY1 = rand() % gridSize;
        message->sender = &nodeGrid[randX1][randY1];

        // Make sure Sender and Reciever are different
        while(!pass) {
            // Randomize Reciever
            randX2 = rand() % gridSize;
            randY2 = rand() % gridSize;

            if(!(randX1 == randX2 && randY1 == randY2))
                pass = true;
        }
        message->reciever = &nodeGrid[randX2][randY2];

        break;
      }
    case DS_ID_MAP:
      {

        break;
      }
    }

    message->packetId = message->sender->packetIndex;
}

void Mesh::sendPacket(Node::packet* message)
{
    currentMessage = message;

    // Set timeout
    time_t tm;
    time(&tm);
    struct tm* to = localtime(&tm);
    to->tm_min += 10; // 10 minute timeout
    message->timout = mktime(to);

    message->sender->forwardPacket(message);
}

void Mesh::resendPacket()
{
    sendPacket(currentMessage);
}

void Mesh::sendAck()
{
    Node::packet* ack = new Node::packet;

    // Set Sender/Reciever
    ack->sender = currentMessage->reciever;
    ack->reciever = currentMessage->sender;

    // Set PacketId (frame number)
    ack->packetId = currentMessage->packetId;

    // Set and reverse path
    ack->path = currentMessage->path;
    reversePath(ack->path);

    // Mark as Ack
    ack->data = "Ack";

    // Set timeout to message timeout
    ack->timout = currentMessage->timout;

    // Send
    ack->sender->forwardPacket(ack);
}


