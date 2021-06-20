#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <fstream>
#include <QDebug>
#include <QDateTime>
#include "core/Biots.h"
#include "rapidjson/writer.h"
#include <rapidjson/ostreamwrapper.h>
using namespace std;
using namespace rapidjson;

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

    Document d;
    d.SetObject();
    Value biotsJson(kArrayType);
    for(int i=0; i<env.m_biotList.size(); i++)
    {
        Value biotJson(kObjectType);
        Biot *biot = env.m_biotList[i];
        biot->SerializeJson(d, biotJson);
        biotsJson.PushBack(biotJson, d.GetAllocator());
    }
    d.AddMember("biots", biotsJson, d.GetAllocator());
    ofstream myfile("example.json");
    OStreamWrapper osw(myfile);
    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);

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
