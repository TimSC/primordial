#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <fstream>
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include "core/Biots.h"
#include "rapidjson/writer.h"
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
using namespace std;
using namespace rapidjson;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QRect rect(0, 0, 2000, 1500);
    QPen whitePen(QColor(255,255,255));
    QGraphicsRectItem *gri = new QGraphicsRectItem(rect);
    gri->setPen(whitePen);
    this->scene.addItem(gri);

    this->env.options.Reset(1000, 600);

    qint64 seed = QDateTime::currentMSecsSinceEpoch();
    //qint64 seed = 100;
    int numBiots = 20;
    this->env.OnNew(this->scene, rect, numBiots, seed,
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

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Address Book"), "",
        tr("Primordial Life Files (*.plfj);;All Files (*)"));

    if (fileName.isEmpty())
            return;

    QRect rect(0, 0, 2000, 1500);

    const Value *parsedBiots = nullptr;
    int numBiots = 20;
    Document d;

    ifstream ifs(fileName.toStdString().c_str());
    IStreamWrapper isw(ifs);

    d.ParseStream(isw);
/*    parsedBiots = &d["biots"];
    numBiots = parsedBiots->Size();

    qint64 seed = QDateTime::currentMSecsSinceEpoch();
    this->env.OnNew(this->scene, rect, numBiots, seed,
                0, 1, 10);

    for(int i=0; i<parsedBiots->Size(); i++)
    {
        Biot *biot = env.m_biotList[i];
        biot->SerializeJsonLoad((*parsedBiots)[i]);
    }*/
    this->env.SerializeJsonLoad(d["environment"]);
    this->env.OnOpen();

}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save As"), "",
        tr("Primordial Life Files (*.plfj);;All Files (*)"));

    if (fileName.isEmpty())
            return;

    Document d;
    d.SetObject();
    Value envJson(kObjectType);
    /*for(int i=0; i<env.m_biotList.size(); i++)
    {
        Value biotJson(kObjectType);
        Biot *biot = env.m_biotList[i];
        biot->SerializeJson(d, biotJson);
        biotsJson.PushBack(biotJson, d.GetAllocator());
    }*/
    this->env.SerializeJson(d, envJson);
    d.AddMember("environment", envJson, d.GetAllocator());
    ofstream myfile(fileName.toStdString().c_str());
    OStreamWrapper osw(myfile);
    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);
}
