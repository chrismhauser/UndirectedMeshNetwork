#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mesh.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Mesh mesh();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_submitButton_clicked()
{
    int* ipNums = new int[4];
    ipNums[0] = atoi(ui->ipInputNum1->text().toStdString().c_str());
    ipNums[1] = atoi(ui->ipInputNum2->text().toStdString().c_str());
    ipNums[2] = atoi(ui->ipInputNum3->text().toStdString().c_str());
    ipNums[3] = atoi(ui->ipInputNum4->text().toStdString().c_str());

    Ip ip(ipNums[0], ipNums[1], ipNums[2], ipNums[3]);

    for(int i=0; i<4; i++)
        std::cout << "Input Num " << i << ": " << ip.getIpNum(i) << std::endl;

    std::cout << "IP: " << ip.getIpString() << std::endl;

    ui->textView->append(QString(ip.getIpString().c_str()));

    ui->ipInputNum1->clear();
    ui->ipInputNum2->clear();
    ui->ipInputNum3->clear();
    ui->ipInputNum4->clear();
}
