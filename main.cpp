#include "mainwindow.h"
#include <QApplication>

#include "tree.h"
#include "mesh.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    std::cout << "Test" << std::endl;
    Mesh myMesh;

    std::cout << "Generate Path" << std::endl;
    std::queue<Ip> path = myMesh.getCurMessage()->path;

    std::cout << "Output Path" << std::endl;
    std::queue<Ip> q = path;
    while(!q.empty())
    {
        std::cout << '\t' << q.front().getIpString() << std::endl;
        q.pop();
    }

    return a.exec();
}
