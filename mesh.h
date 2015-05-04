#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <utility>
#include <time.h>

#include "ip.h"
#include "node.h"
#include "tree.h"

#define DS_ID_GRID 0
#define DS_ID_MAP 1

class Mesh : public QObject
{
    Q_OBJECT
private:
    Node** nodeGrid;
    int gridSize;
    std::ofstream out;
    tree syncTree;
    std::queue<std::pair<Node*, Node*>> treeBuildQue;

    std::unordered_map<Ip, Node*> nodeMap;
    int mapSize;

    void generateGrid(size_t size = 2);
    void generateMap(size_t size = 75);

    void generateTree(Node* head);
    void generatePath();
    void generateLogs(int path_count);



    void reversePath(std::queue<Ip>& path);

    Node::packet* currentMessage;
    int CUR_DS_ID;

    void log(std::string& s);

public:
    Mesh();
    ~Mesh();
    Mesh(size_t ds_id, size_t size);

    std::queue<Ip> findPath(Node* sender, Node* reciever);
    Node::packet* generatePacket();
    void sendPacket(Node::packet* message);

    Node::packet* getCurMessage();

public slots:
    // on signal packetDiscarded
    void resendPacket();

    // on signal packetRecieved
    void sendAck();
};

#endif // MESH_H
