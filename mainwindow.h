#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cstdlib>
#include <iostream>
#include <string.h>

#include "mesh.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_submitButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
