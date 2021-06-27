#include "environmentarea.h"
#include "core/Biots.h"
#include <QPainter>
#include <QDateTime>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include "rapidjson/writer.h"
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>

using namespace std;
using namespace rapidjson;

EnvironmentArea::EnvironmentArea(QWidget *central) : QOpenGLWidget(central)
{
    this->env = nullptr;

    tickStart = QDateTime::currentMSecsSinceEpoch();
    tickCount = 0;
    ticksPerSec = 0.0;
}

void EnvironmentArea::SetEnvironment(class Environment *envIn)
{
    this->env = envIn;
}

void EnvironmentArea::paintEvent(QPaintEvent* event)
{
   //makeCurrent();
   //paintGL();
   QOpenGLWidget::paintEvent(event);
}

void EnvironmentArea::resizeEvent(QResizeEvent* event)
{
    //your code...
    QOpenGLWidget::resizeEvent(event);
}

void EnvironmentArea::paintGL()
{
    uint64_t tickNow = QDateTime::currentMSecsSinceEpoch();
    uint64_t elapse = tickNow - tickStart;
    if(elapse > 1000)
    {
        //std::cout << ticksPerSec << std::endl;
        ticksPerSec = ((double)tickCount / (double)elapse) * 1000.0;
        tickStart = tickNow;
        tickCount = 0;
    }
    tickCount ++;

    QPainter painter(this);
    painter.setPen(QPen(Qt::blue));

    this->env->paintGL(painter);
}

void EnvironmentArea::mousePressEvent(QMouseEvent * event)
{
    int x = event->x();
    int y = event->y();
    Biot *pBiot = this->env->FindBiotByPoint(x, y);
    if(pBiot == nullptr and currentTool != "open") return;

    if(currentTool == "cure-sicken")
    {
        if (pBiot->m_nSick == 0)
        {
            //PlayResource("PL.TooOld");
            pBiot->newType = PURPLE_LEAF;
            pBiot->m_nSick = 200;
        }
        else
        {
            pBiot->newType = -1;
            pBiot->m_nSick = 0;
        }
    }
    else if (currentTool == "examine")
    {
        this->env->SetSelectedBiot(pBiot->m_Id);
    }
    else if (currentTool == "feed")
    {
        //PlayResource("PL.Feed");
        pBiot->energy += pBiot->childBaseEnergy;
        pBiot->newType = GREEN_LEAF;
    }
    else if (currentTool == "mutate")
    {
        //PlayResource("PL.Edit");

        pBiot->Mutate(100);
        pBiot->newType = WHITE_LEAF;
        pBiot->Initialize();
    }
    else if (currentTool == "open")
    {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open Address Book"), "",
            tr("Primordial Life Files (*.plfj);;All Files (*)"));

        if (fileName.isEmpty())
                return;

        Document d;

        ifstream ifs(fileName.toStdString().c_str());
        IStreamWrapper isw(ifs);

        d.ParseStream(isw);
        pBiot = new Biot(*env);
        pBiot->SerializeJsonLoad(d["biot"]);
        pBiot->Place(x, y);
        pBiot->OnOpen();
        env->AddBiot(pBiot);
    }
    else if (currentTool == "relocate")
    {

    }
    else if (currentTool == "save")
    {
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save As"), "",
            tr("Primordial Life Files (*.plfj);;All Files (*)"));

        if (fileName.isEmpty())
                return;

        Document d;
        d.SetObject();
        Value biotJson(kObjectType);
        pBiot->SerializeJson(d, biotJson);
        d.AddMember("biot", biotJson, d.GetAllocator());
        ofstream myfile(fileName.toStdString().c_str());
        OStreamWrapper osw(myfile);
        Writer<OStreamWrapper> writer(osw);
        d.Accept(writer);
    }
    else if (currentTool == "terminate")
    {
        //PlayResource("PL.Terminate");
        pBiot->newType = YELLOW_LEAF;
        pBiot->m_age = pBiot->m_maxAge;
    }

}

void EnvironmentArea::mouseReleaseEvent(QMouseEvent * event)
{
    int x = event->x();
    int y = event->y();

}

void EnvironmentArea::setCurrentTool(const std::string &tool)
{
    currentTool = tool;
}
