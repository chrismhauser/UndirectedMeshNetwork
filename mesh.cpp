
#include "mesh.h"

Mesh::Mesh() {

    // Seed rand for more realisticly random data
    // srand(time(NULL));
    Mesh(0,5,0,5,"log.txt");


}

Mesh::Mesh(size_t ds_id, size_t size, size_t alg_id, size_t max_weight, std::string outputFile)
{
    // Clear file
    out.open(outputFile.c_str(), std::ios::out);
    out.close();
    out.open(outputFile.c_str(), std::ios::app);
    if(!out.is_open())
        std::cout << "Error opening log file" << std::endl;
    else {
        std::cout << "File Open" << std::endl;
    }

    this->max_weight = max_weight;

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

    CUR_ALG_ID = alg_id;

    // TEST STATEMENT
    //std::cout << "Generate Packet called." << std::endl;

    //generatePath();
    // currentMessage = generatePacket();

    // TEST STATEMENT
    // std::cout << "Send packet called." << std::endl;

    // sendPacket(currentMessage);
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
                srand(time(NULL));
                nodeGrid[i][j].connectorWeights.push_back(rand() % max_weight);

                nodeGrid[i-1][j].connectedNodes.push_back(&nodeGrid[i][j]);
                srand(time(NULL));
                nodeGrid[i-1][j].connectorWeights.push_back(rand() % max_weight);
            }

            if(j > 0) {
                // connect both ways
                nodeGrid[i][j].connectedNodes.push_back(&nodeGrid[i][j-1]);
                srand(time(NULL));
                nodeGrid[i][j].connectorWeights.push_back(rand() % max_weight);

                nodeGrid[i][j-1].connectedNodes.push_back(&nodeGrid[i][j]);
                srand(time(NULL));
                nodeGrid[i][j-1].connectorWeights.push_back(rand() % max_weight);
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
                 srand(time(NULL));
                 if ((rand() % 8 + 1) > 5)
                    tempNodeVect[i][j]->connectedNodes.push_back(&nodeGrid[i-1][j]);

                 srand(time(NULL));
                 if ((rand() % 8 + 1) > 5)
                    tempNodeVect[i-1][j]->connectedNodes.push_back(&nodeGrid[i][j]);
             }

             if (j > 0) {
                 srand(time(NULL));
                 if ((rand() % 8 + 1) > 5)
                    tempNodeVect[i][j]->connectedNodes.push_back(&nodeGrid[i][j-1]);

                 srand(time(NULL));
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

void Mesh::generatePath()
{
    std::cout << "generatePath #1.  Tree begins" << std::endl;
    Node::packet* message = currentMessage;

    // TEST STATEMENT
    std::cout << "Sender: " << message->sender->address.getIpString() << " ";
    std::cout << "Reciever: " << message->reciever->address.getIpString() << std::endl;

    switch(CUR_ALG_ID)
    {
    case ALG_ID_SP:
    {
        syncTree.setHead(message->sender->address);
        generateBuildQue(message->sender);

        message->path = syncTree.findPath(message->reciever->address);
        break;
    }
    case ALG_ID_RNG:
    {
        // Route from Sender to Interm
        syncTree.setHead(message->sender->address);
        generateBuildQue(message->sender);

        message->path = syncTree.findPath(message->interm->address);

        // Route from Interm to Reciever
        syncTree.setHead(message->interm->address);

        if (treeBuildQue.empty())
            std::cout << "Empty" <<std::endl;


        generateBuildQue(message->interm);
        std::queue<Ip> tempPath = syncTree.findPath(message->reciever->address);

        // Combine paths
        // Pop off interm node (already on path)
        tempPath.pop();
        while(!tempPath.empty())
        {
            message->path.push(tempPath.front());
            tempPath.pop();
        }

        std::cout << "Interm: " << message->interm->address.getIpString() << std::endl;

        break;
    }
    case ALG_ID_SUB:
    {
        // Route from Sender to Interm
        syncTree.setHead(message->sender->address);
        generateBuildQue(message->sender);

        message->path = syncTree.findPath(message->interm->address);

        // Route from Interm to Reciever
        syncTree.setHead(message->interm->address);

        if (treeBuildQue.empty())
            std::cout << "Empty" <<std::endl;


        generateBuildQue(message->interm);
        std::queue<Ip> tempPath = syncTree.findPath(message->reciever->address);

        // Combine paths
        // Pop off interm node (already on path)
        tempPath.pop();
        while(!tempPath.empty())
        {
            message->path.push(tempPath.front());
            tempPath.pop();
        }

        std::cout << "Interm: " << message->interm->address.getIpString() << std::endl;

        break;
    }
    }

    std::queue<Ip> q = message->path;

    // TEST STATEMENT
    std::cout << "Test: Popping off the message path que." << std::endl;
    while(!q.empty()) {
        std::cout << q.front().getIpString() << std::endl;
        q.pop();
    }
}

Node::packet* Mesh::generatePacket()
{
    /* TODO
    *   choose random sender and reciever (grid - done, map - done)
    *   set packetId
    *   set random data (for loop random # of times, str += "data")
    */
    // TEST STATEMENT
    std::cout << "generatePacket #1.  Tree begins" << std::endl;
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
        srand(time(NULL));
        randX1 = rand() % gridSize;
        srand(time(NULL));
        randY1 = rand() % gridSize;

        message->sender = &nodeGrid[randX1][randY1];

        // Make sure Sender and Reciever are different
        while(!pass) {
            // Randomize Reciever
            srand(time(NULL));
            randX2 = rand() % gridSize;
            srand(time(NULL));
            randY2 = rand() % gridSize;

            if(!(randX1 == randX2 && randY1 == randY2))
                pass = true;
        }
        message->reciever = &nodeGrid[randX2][randY2];

        switch(CUR_ALG_ID)
        {
        case ALG_ID_RNG:
        {
            int randX3 = rand() % gridSize;
            int randY3 = rand() % gridSize;

            message->interm = &nodeGrid[randX3][randY3];
            break;
        }
        case ALG_ID_SUB:
        {
            int smallX = randX1;
            int smallY = randY1;

            int bigX = randX2;
            int bigY = randY2;

            if(randX2 < smallX) {
                smallX = randX2;
                bigX = randX1;
            }

            if(randY2 < smallY) {
                smallY = randY2;
                bigY = randY1;
            }

            int randX3 = smallX + (rand() % (bigX-smallX));
            int randY3 = smallY + (rand() % (bigY-smallY));

            message->interm = &nodeGrid[randX3][randY3];
            break;
        }
        }

        break;
      }
    case DS_ID_MAP:
      {
        bool pass = false;
        auto randTuple1 = nodeMap.begin();
        auto randTuple2 = nodeMap.begin();
        int advanceCount1, advanceCount2;

         // Randomize sender
        srand(time(NULL));
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

    // TEST STATEMENT
    std::cout << "Generate Packet #2.  (Random sender and reciever successful). ";
    std::cout << "Sender: " << message->sender->address.getIpString() << " ";
    std::cout << "Reciever: " << message->reciever->address.getIpString() << std::endl;

    message->packetId = message->sender->packetIndex;

    // Generate random data
    srand(time(NULL));
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

//    // TEST STATEMENT
//    std::cout << "Generate Packet #3.  Building treeBuildQue." << std::endl;
//    syncTree.setHead(message->sender->address);
//    generateBuildQue(message->sender);

//    // TEST STATEMENT
//    std::cout << "Generate Packet #5.  GenerateTree called successfully. Tree print:" << std::endl;
//    syncTree.print();

//    message->path = syncTree.findPath(message->reciever->address);
//    std::queue<Ip> q = message->path;

//    // TEST STATEMENT
//    std::cout << "Test: Popping off the message path que." << std::endl;
//    while(!q.empty()) {
//        std::cout << q.front().getIpString() << std::endl;
//        q.pop();
//    }
    return message;
}

void Mesh::simulateFlow() {

    srand(time(NULL));

    std::cout << "Generate Packet" << std::endl;
    currentMessage = generatePacket();
    currentMessage->packetId = 0;

    std::cout << "Generate Path" << std::endl;
    generatePath();

    time(&currentMessage->sendTime);

    size_t numFrames = (rand()%5)+1;
    std::cout << "Num frames: " << numFrames << std::endl;
    for(int i=0; i<numFrames; i++) {
        sendPacket();
        currentMessage->packetId++;
    }

}

void sleep(int numSecs) {
    clock_t now, stopTime;
    time(&stopTime);
    stopTime += numSecs;
    time(&now);
    while(now < stopTime) {
        time(&now);
    }
}

void Mesh::sendPacket()
{
    out << "SEND_PACKET " << currentMessage->packetId << std::endl;
    std::cout << "SEND PACKET " << currentMessage->packetId << std::endl;

    out << "Hops " << currentMessage->path.size()-1 << std::endl;
    std::cout << "Hops " << currentMessage->path.size()-1 << std::endl;
    time_t arriveTime, departTime;

    /* 4 Type of Delay (* = not for sender)(** = not for reciever)
     *  Queueing*
     *  Processing*
     *  Transmission**
     *  Propogation**
     */

    int queueing, processing, transmission, propogation;

    std::queue<Ip> path = currentMessage->path;

    srand(time(NULL));
    int randDec;

    // Randomize delay at each node on path
    while(!path.empty())
    {
        time(&arriveTime);
        randDec = rand() % 10000;
        out << "Arrives @ " << path.front().getIpString() << ' ' << difftime(arriveTime,currentMessage->sendTime) << '.' << randDec << std::endl;
        std::cout << "Arrives @ " << path.front().getIpString() << ' ' << difftime(arriveTime,currentMessage->sendTime) << '.' << randDec << std::endl;

        // Queing Delay
        if(path.front() != currentMessage->sender->address)
        {
            std::cout << "\tBegin queueing" << std::endl;

            srand(time(NULL));
            queueing = (rand() % queueRange) + 1;
            sleep(queueing);

            std::cout << "\t\tFinished queueing (" << queueing << " sec)" << std::endl;
        }

        // Processing Delay
        if(path.front() != currentMessage->sender->address)
        {
            std::cout << "\tBegin processing" << std::endl;

            srand(time(NULL));
            processing = (rand() % procRange) + 1;
            sleep(processing);

            std::cout << "\t\tFinished processing (" << processing << " sec)" << std::endl;
        }

        // Transmission Delay
        if(path.front() != currentMessage->reciever->address)
        {
            std::cout << "\tBegin transmission" << std::endl;

            srand(time(NULL));
            transmission = (rand() % transRange) + 1;
            transmission = currentMessage->data.size() / (transmission*2);
            sleep(transmission);

            std::cout << "\t\tFinished transmission (" << transmission << " sec)" << std::endl;
            time(&departTime);
            randDec = rand() % 10000;
            out << "Departs @ " << path.front().getIpString() << ' ' << difftime(departTime,currentMessage->sendTime) << '.' << randDec << std::endl;
            std::cout << "Departs @ " << path.front().getIpString() << ' ' << difftime(departTime,currentMessage->sendTime) << '.' << randDec << std::endl;
        }

        // Propogation Delay
        if(path.front() != currentMessage->reciever->address)
        {
            tree::node* par = syncTree.findNode(path.front());

            // Pop off front
            path.pop();

            std::cout << "\tBegin propogation to Node " << path.front().getIpString() << std::endl;

            tree::node* child = syncTree.findNode(path.front());
            int connectionWeight = child->weight - par->weight;

            srand(time(NULL));
            propogation = (rand() % propRange) + 1;
            propogation = (connectionWeight / propogation) + 1;
            sleep(propogation);

            std::cout << "\t\tFinished propogation (" << propogation << " sec)" << std::endl;
        }
        else // Packet Arrives at reciever
        {
//            time_t finalRecieveTime;
//            time(&finalRecieveTime);
//            out << "Arrives @ " << path.front().getIpString() << ' ' << difftime(finalRecieveTime,currentMessage->sendTime) << std::endl;
//            std::cout << "Arrives @ " << path.front().getIpString() << ' ' << difftime(finalRecieveTime,currentMessage->sendTime) << std::endl;
            respondWithAck();
            path.pop();
        }


    }
}

void Mesh::respondWithAck()
{
    time_t arriveTime, departTime;
    out << "SEND_ACK " << currentMessage->packetId << std::endl;
    std::cout << "SEND_ACK " << currentMessage->packetId << std::endl;

    out << "Hops " << currentMessage->path.size()-1 << std::endl;
    std::cout << "Hops " << currentMessage->path.size()-1 << std::endl;

    /* 4 Type of Delay (* = not for sender (Ack reciever))(** = not for reciever (Ack sender))
     *  Queueing*
     *  Processing*
     *  Transmission**
     *  Propogation**
     */

    int queueing, processing, transmission, propogation;

    std::queue<Ip> path = currentMessage->path;
    reversePath(path);

    int randDec;

    // Randomize delay at each node on path
    while(!path.empty())
    {
        time(&arriveTime);
        randDec = rand() % 10000;
        out << "Arrives @ " << path.front().getIpString() << ' ' << difftime(arriveTime,currentMessage->sendTime) << '.' << randDec << std::endl;
        std::cout << "Arrives @ " << path.front().getIpString() << ' ' << difftime(arriveTime,currentMessage->sendTime) << '.' << randDec << std::endl;

        // Queing Delay
        if(path.front() != currentMessage->reciever->address)
        {
            std::cout << "\tBegin queueing" << std::endl;

            srand(time(NULL));
            queueing = (rand() % queueRange) + 1;
            sleep(queueing);

            std::cout << "\t\tFinished queueing (" << queueing << " sec)" << std::endl;
        }

        // Processing Delay
        if(path.front() != currentMessage->reciever->address)
        {
            std::cout << "\tBegin processing" << std::endl;

            srand(time(NULL));
            processing = (rand() % procRange) + 1;
            sleep(processing);

            std::cout << "\t\tFinished processing (" << processing << " sec)" << std::endl;
        }

        // Transmission Delay
        if(path.front() != currentMessage->sender->address)
        {
            std::cout << "\tBegin transmission" << std::endl;

            srand(time(NULL));
            transmission = (rand() % transRange) + 1;
            transmission = currentMessage->data.size() / (transmission*2);

            sleep(transmission);

            std::cout << "\t\tFinished transmission (" << transmission << " sec)" << std::endl;

            time(&departTime);
            randDec = rand() % 10000;
            out << "Departs @ " << path.front().getIpString() << ' ' << difftime(departTime,currentMessage->sendTime) << '.' << randDec << std::endl;
            std::cout << "Departs @ " << path.front().getIpString() << ' ' << difftime(departTime,currentMessage->sendTime) << '.' << randDec << std::endl;
        }

        // Propogation Delay
        if(path.front() != currentMessage->sender->address)
        {
            tree::node* par = syncTree.findNode(path.front());

            // Pop off front
            path.pop();

            std::cout << "\tBegin propogation to Node " << path.front().getIpString() << std::endl;

            tree::node* child = syncTree.findNode(path.front());
            int connectionWeight = par->weight - child->weight;

            srand(time(NULL));
            propogation = (rand() % propRange) + 1;
            propogation = (connectionWeight / propogation) + 1;
            sleep(propogation);

            std::cout << "\t\tFinished propogation (" << propogation << " sec)" << std::endl;
        }
        else
        {
//            time_t finalRecieveTime;
//            time(&finalRecieveTime);
//            out << "Arrives @ " << path.front().getIpString() << ' ' << difftime(finalRecieveTime,currentMessage->sendTime) << std::endl;
//            std::cout << "Arrives @ " << path.front().getIpString() << ' ' << difftime(finalRecieveTime,currentMessage->sendTime) << std::endl;
            path.pop();
            return;
        }


    }
}

int Mesh::findConnectorWeight(Ip a, Ip b)
{
    switch(CUR_DS_ID)
    {
    case DS_ID_GRID:
    {
        for(int i=0; i<nodeGrid[a.getIpNum(2)][a.getIpNum(3)].connectedNodes.size(); i++)
        {
            if(nodeGrid[a.getIpNum(2)][a.getIpNum(3)].connectedNodes.at(i)->address == b)
            {
                return nodeGrid[a.getIpNum(2)][a.getIpNum(3)].connectorWeights.at(i);
            }
        }
        break;
    }
    case DS_ID_MAP:
    {
        for(int i=0; i<nodeMap[a]->connectedNodes.size(); i++)
        {
            if(nodeMap[a]->connectedNodes.at(i)->address == b)
            {
                return nodeMap[a]->connectorWeights.at(i);
            }
        }
        break;
    }
    }
}

Node::packet* Mesh::getCurMessage()
{
    return currentMessage;
}

void Mesh::resendPacket()
{
    sendPacket();
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

// Call this to begin recursive tree generation.  BROKEN FUCK THIS SHIT
void Mesh::generateTree(Node* node)
{
    // TEST STATEMENT
    std::cout << node->address.getIpString() << std::endl;

    // Remove top peer (if it exists) to avoid infinite loop
    if (node->address.getIpNum(2) > 0) {
        // Only remove if were not on the edges, otherwise connectedNodes order is different
        if (!(node->address.getIpNum(3) == 0) && !(node->address.getIpNum(3) == 4)) {
            // it's stored at the connectedNodes index 0, hence begin
            auto rmv = node->connectedNodes.begin();
            std::cout << "Removing top connection for infinite recursion" << std::endl;
            node->connectedNodes.erase(rmv);
        }
    }

    // add children to the tree
    for (size_t i = 0; i < node->connectedNodes.size(); i++)
    {
        // Remove the parent from the child's connectedNodes to avoid infinite loop
        auto it = node->connectedNodes.at(i)->connectedNodes.begin();
        auto end = node->connectedNodes.at(i)->connectedNodes.end();

        for(;it != end; it++) {
            if ((*it)->address.getIpString() == node->address.getIpString()) {
                std::cout << "Removing parent for infinite recursion" << std::endl;
                node->connectedNodes.at(i)->connectedNodes.erase(it);
            }
        }

        // Insert a child of node (use findNode function to insert into the
        // appropriate parent).
        // Params 2 and 3 are the node's IP address and weight.
        syncTree.insertChild(syncTree.findNode(node->address),
                             node->connectedNodes.at(i)->address,
                             node->connectorWeights.at(i));

        generateTree(node->connectedNodes.at(i));
    }
}

void Mesh::generateBuildQue(Node* head)
{
    std::unordered_map<Node*, bool> isParent;
    std::vector<Node*> buffer[2];
    bool allMapped = false;
    int current = 0;
    int next = 1;

    // Establish first level using children of head
    for (size_t i = 0; i < head->connectedNodes.size(); ++i)
    {
        buffer[current].push_back(head->connectedNodes.at(i));
        treeBuildQue.push(std::make_pair(head, head->connectedNodes.at(i)));
    }
    isParent[head] = true;

    while(true)
    {
        allMapped = true;
        // For parent in buffer
        for (size_t i = 0; i < buffer[current].size(); ++i) {

            // For child in parent
            for (size_t j = 0; j < buffer[current][i]->connectedNodes.size(); ++j)
            {
                // If we havn't yet iterated through it's parents
                if (isParent.count(buffer[current][i]->connectedNodes[j]) == 0)
                {
                    // add pair (parent, child) to build que
                    treeBuildQue.push(std::make_pair(buffer[current][i],
                                                     buffer[current][i]->connectedNodes[j]));
                    // add child to the next buffer
                    buffer[next].push_back(buffer[current][i]->connectedNodes[j]);
                    allMapped = false;
                }
            }
            // add parent to isParent
            isParent[buffer[current][i]] = true;
        }
        // If none of the parents yielded children to add, then break
        // If none of the parents yielded children to add, then break
        if (allMapped == true) break;
        buffer[current].clear();
        std::swap(current, next);
    }
    generateTree();
}

void Mesh::generateTree()
{
    while (!treeBuildQue.empty())
    {
        //std::cout << treeBuildQue.front().first->address.getIpString() << ", ";
       // std::cout << treeBuildQue.front().second->address.getIpString() << std::endl;

        // Insert a child of node (use findNode function to insert into the
        // appropriate parent).
        // Params 2 and 3 are the node's IP address and weight.
        srand(time(NULL));
        syncTree.insertChild(syncTree.findNode(treeBuildQue.front().first->address),
                             treeBuildQue.front().second->address,
                             findConnectorWeight(treeBuildQue.front().first->address,treeBuildQue.front().second->address));
        treeBuildQue.pop();
    }
}

Mesh::~Mesh() {
    out.close();
}
