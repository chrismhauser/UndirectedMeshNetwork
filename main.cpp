#include "mainwindow.h"
#include <QApplication>

#include "tree.h"
#include "mesh.h"

int tSize[2] = {5,10};
int fSize[2] = {3,5};
int algId[3] = {0,1,2};
int maxWeight[2] = {5,10};

void testAlgos() {


    // Pick Algo
    for(int i=0; i<3; i++)
    {
        // Pick Topology Size
        for(int j=0; j<2; j++)
        {
            // Pick Max Weight
            for(int k=0; k<2; k++)
            {
                // Pick Flow Size
                for(int l=0; l<2; l++)
                {
                    std::string fileName = "log";

                    fileName += char(i+'0');


                    fileName += char(j+'0');


                    fileName += char(k+'0');


                    fileName += char(l+'0');

                    fileName += ".txt";
                    std::cout << "FILE " << fileName << std::endl;
                    Mesh myMesh(0,tSize[j],i,maxWeight[k], fileName);
                    for(int n=0; n<fSize[l]; n++)
                    {
                        std::cout << "Simulate Flow " << n << std::endl;
                        myMesh.simulateFlow();
                    }
                }
            }
        }

    }

//    size_t flowSize = 5;
//    for(int i=0; i<flowSize; i++) {
//        std::cout << "Sending Packet" << std::endl;
//        myMesh.simulateFlow();
//    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    testAlgos();

    return 0;
}
