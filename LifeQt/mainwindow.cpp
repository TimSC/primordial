#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <fstream>
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include <QOpenGLWidget>
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

    if(1)
    {
        QOpenGLWidget *gl = new QOpenGLWidget();
        QSurfaceFormat format;
        //format.setSamples(16);
        gl->setFormat(format);
        this->ui->graphicsView->setViewport(gl);
    }

    QRect rect(0, 0, 2000, 1500);
    QPen whitePen(QColor(255,255,255));
    QGraphicsRectItem *gri = new QGraphicsRectItem(rect);

    gri->setPen(whitePen);
    this->scene.addItem(gri);

    this->env.options.Reset(rect.x(), rect.y());

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
        tr("Open Address Book"), this->currentFilename.c_str(),
        tr("Primordial Life Files (*.plfj);;All Files (*)"));

    if (fileName.isEmpty())
            return;

    Document d;

    ifstream ifs(fileName.toStdString().c_str());
    IStreamWrapper isw(ifs);

    d.ParseStream(isw);
    this->env.SerializeJsonLoad(d["environment"]);
    this->env.OnOpen();

}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save As"), this->currentFilename.c_str(),
        tr("Primordial Life Files (*.plfj);;All Files (*)"));

    if (fileName.isEmpty())
            return;

    this->currentFilename = fileName.toStdString();
    Document d;
    d.SetObject();
    Value envJson(kObjectType);
    this->env.SerializeJson(d, envJson);
    d.AddMember("environment", envJson, d.GetAllocator());
    ofstream myfile(fileName.toStdString().c_str());
    OStreamWrapper osw(myfile);
    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);
}

void MainWindow::on_actionNew_triggered()
{
    QRect rect(0, 0, 2000, 1500);
    qint64 seed = QDateTime::currentMSecsSinceEpoch();
    int numBiots = 20;

    this->env.DeleteContents();
    this->env.OnNew(this->scene, rect, numBiots, seed,
                                  0, 1, 10);
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionSave_triggered()
{
    if (this->currentFilename.empty())
    {
        on_actionSave_As_triggered();
        return;
    }

    Document d;
    d.SetObject();
    Value envJson(kObjectType);
    this->env.SerializeJson(d, envJson);
    d.AddMember("environment", envJson, d.GetAllocator());
    ofstream myfile(this->currentFilename);
    OStreamWrapper osw(myfile);
    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);
}

void MainWindow::on_actionStart_Simulation_triggered()
{

}

void MainWindow::on_actionSettings_triggered()
{

}

void MainWindow::on_actionLearn_about_Primordial_Life_triggered()
{

}
