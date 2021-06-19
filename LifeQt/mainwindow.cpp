#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QDebug>
#include <QDateTime>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRect rect(0, 0, 1080, 760);

    this->env.options.Reset(640, 480);

    this->env.OnNew(this->scene, rect, 20, QDateTime::currentMSecsSinceEpoch(),
                0, 1, 10);

    this->ui->graphicsView->setScene(&this->scene);

    startTimer(50);     // 50-millisecond timer
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    cout << this->env.GetPopulation() << endl;
    this->env.Skip();
}
