
#include "mesh.h"

Mesh::Mesh(){
    std::cout << "HERE" << std::endl;
    Mesh(0,20);
    std::cout << "HERE2" << std::endl;
    out.open("log.txt");
    if(!out.is_open())
        std::cout << "Error opening log file" << std::endl;
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

    std::cout << "HURRRR" << std::endl;
    currentMessage = generatePacket();
    std::cout << "HURRRR2" << std::endl;
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

// Generates a map of approximately size ^ 2 nodes (no size guarantee)
void Mesh::generateMap(size_t size /*= 75*/)
{
    /* TODO generate map of nodes (default size = 75)
     *  assign Ip addresses
     *  connect node signals to mesh slots
     */

    mapSize = size ^ 2;
    std::vector<std::vector<Node*>> tempNodeVect;

    // initialize size x size vector of nodes
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            Node* myNode = new Node;
            tempNodeVect[i].push_back(myNode);
        }
    }

    // ip assigning occurs the same as the mesh abstraction (shout out Chris H.)
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
             tempNodeVect[i][j]->address.setIp(0,0,i,j);

             // Unlike mesh, asymetric graph edges.  Average of 2.5 edges between a node and its peers.
             // Note that with the randomization strategy, there is a small possibility of isolated nodes.
             if (i > 0) {
                 // .625% chance of connection with neighbor
                 if ((rand() % 8 + 1) > 5)
                    tempNodeVect[i][j]->connectedNodes.push_back(&nodeGrid[i-1][j]);
                 if ((rand() % 8 + 1) > 5)
                    tempNodeVect[i-1][j]->connectedNodes.push_back(&nodeGrid[i][j]);
             }

             if (j > 0) {
                 if ((rand() % 8 + 1) > 5)
                    tempNodeVect[i][j]->connectedNodes.push_back(&nodeGrid[i][j-1]);
                 if ((rand() % 8 + 1) > 5)
                    tempNodeVect[i][j-1]->connectedNodes.push_back(&nodeGrid[i][j]);
             }
        }
    }

    // Iterate through tempNodeVect and push non-isolated nodes to the map
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (tempNodeVect[i][j]->connectedNodes.size() > 0) {
                nodeMap[tempNodeVect[i][j]->address] = tempNodeVect[i][j];
            }
        }
  }
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

std::queue<Ip> Mesh::findPath(Node *sender, Node *reciever)
{
    std::stack<Ip> s;
    generateTree(sender);
    return syncTree.findPath(reciever->address);
}

Node::packet* Mesh::generatePacket()
{
    /* TODO
    *   choose random sender and reciever (grid - done, map - done)
    *   set packetId
    *   set random data (for loop random # of times, str += "data")
    */

    std::cout << "TEST!" << std::endl;
    Node::packet* message = new Node::packet;

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
        bool pass = false;
        auto randTuple1 = nodeMap.begin();
        auto randTuple2 = nodeMap.begin();
        int advanceCount1, advanceCount2;

         // Randomize sender
        advanceCount1 = rand() % nodeMap.size();
        std::advance(randTuple1, advanceCount1);
        message->sender = randTuple1->second;

        // Different sender and reciever
        while(!pass) {
            // Randomize reciever
            advanceCount2 = rand() % nodeMap.size();
            std::advance(randTuple2, advanceCount2);

            if(!(advanceCount1 != advanceCount2))
                pass = true;
        }
        message->reciever = randTuple2->second;
      }
    }
    std::cout << "TEST!2" << std::endl;

    message->packetId = message->sender->packetIndex;

    // Generate random data
    for(int i=0; i<rand()%80; i++) {
        char base;
        switch(rand()%1)
        {
        case 0:
            base = 'a';
            break;
        case 1:
            base = 'A';
            break;
        }

        message->data += base + rand()%26;
    }

    std::cout << "GURRRR" << std::endl;
    // Set the syncTree head to the source node
    syncTree.setHead(message->sender->address);
    std::stack<Ip> s;
    s.push(message->sender->address);
    std::cout << "GURRRR2" << std::endl;
    generateTree(message->sender);
    std::cout << "GURRRR3" << std::endl;

    std::queue<Ip> q = message->path;
    std::cout << "TESTING" << std::endl;
    while(!q.empty()) {

        std::cout << q.front().getIpString() << std::endl;
        q.pop();
    }

    message->path = syncTree.findPath(message->reciever->address);

//    std::queue<Ip> tempPath;

//    Node* tempNode = message->sender;
//    for(int i=0; i<4; i++) {
//        tempPath.push(tempNode->address);
//        tempNode = tempNode->connectedNodes.front();
//    }
//    tempPath.push(tempNode->address);
//    message->path = tempPath;

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

Node::packet* Mesh::getCurMessage()
{
    return currentMessage;
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

// Call this to begin recursive tree generation
void Mesh::generateTree(Node* node)
{
    // TEST
    std::cout << "generateTree()" << std::endl;

    // add children to the tree
    for (size_t i = 0; i < node->connectedNodes.size(); i++)
    {
        // Insert a child of node (use findNode function to insert into the
        // appropriate parent.
        // Params 2 and 3 are the node's IP address and weight.

        // Check if node is in tree

std::cout << node->connectedNodes.at(i)->address.getIpString() << std::endl;
        // Add child to tree
        syncTree.insertChild(syncTree.findNode(node->address),
                             node->connectedNodes.at(i)->address,
                             node->connectorWeights.at(i));

        // check if any of the node's children have already been added
        //  - if not, add them
        //  - else, continue
        if(node->connectedNodes.at(i)->connectedNodes.empty())
            generateTree(node->connectedNodes.at(i));
    }
}

void Mesh::log(std::string& s)
{

}

Mesh::~Mesh() {
    out.close();
}

