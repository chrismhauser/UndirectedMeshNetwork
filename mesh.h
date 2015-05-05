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
#define ALG_ID_SP 0
#define ALG_ID_RNG 1
#define ALG_ID_SUB 2

class Mesh : public QObject
{
    Q_OBJECT
private:
    Node** nodeGrid;
    int gridSize;
    std::ofstream out;
    tree syncTree, syncTree2;
    std::queue<std::pair<Node*, Node*>> treeBuildQue;

    // Gen tree stuff
    int genTreeCount;
    Node* recurLoopCheck;

    std::unordered_map<Ip, Node*> nodeMap;
    int mapSize;

    void generateGrid(size_t size = 5);
    void generateMap(size_t size = 75);

    void generateTree(Node* node);
    void generatePath();
    void generateLogs(int path_count);

    // New attempt at tree building function
    void generateBuildQue(Node* node);
    void generateTree();


    void reversePath(std::queue<Ip>& path);

    Node::packet* currentMessage;
    size_t CUR_DS_ID, CUR_ALG_ID;
    size_t max_weight;

    void log(std::string& s);

    int queueRange = 3;
    int procRange = 3;
    int transRange = 2;
    int propRange = 4;
    int rangeMod = 100000;

public:
    Mesh();
    ~Mesh();
    Mesh(size_t ds_id, size_t size, size_t alg_id, size_t max_weight, std::string outputFile);

    std::queue<Ip> findPath(Node* sender, Node* reciever);
    Node::packet* generatePacket();
    void simulateFlow();
    void sendPacket();
    void respondWithAck();

    int findConnectorWeight(Ip a, Ip b);

    Node::packet* getCurMessage();

public slots:
    // on signal packetDiscarded
    void resendPacket();

    // on signal packetRecieved
    void sendAck();
};

#endif // MESH_H
