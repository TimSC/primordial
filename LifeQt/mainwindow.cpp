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

    QRect rect(0, 0, 1000, 600);

    this->env.options.Reset(1000, 600);

    qint64 seed = QDateTime::currentMSecsSinceEpoch();
    //qint64 seed = 100;
    this->env.OnNew(this->scene, rect, 20, seed,
                0, 1, 10);

    this->ui->graphicsView->setScene(&this->scene);

    startTimer(1);     // 1-millisecond timer
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    //cout << this->env.GetPopulation() << endl;
    this->env.Skip();
}
