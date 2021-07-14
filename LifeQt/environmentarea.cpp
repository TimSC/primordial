#include "environmentarea.h"
#include "core/Biots.h"
#include <QPainter>
#include <QDateTime>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include "core/json.h"
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

    backgroundTop.load(":/res/top.png");
    backgroundBottom.load(":/res/bottom.png");
    currentTool = "examine";
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
    this->paintBackground(painter);


    this->env->paintGL(painter);
}

void EnvironmentArea::paintBackground(QPainter &painter)
{
    painter.save();

    QLinearGradient background(0, 0, 0, this->height());
    background.setColorAt(0, QColor(0,0,100));
    background.setColorAt(1, Qt::black);
    painter.fillRect(this->rect(), background);

    //Repeat texture https://stackoverflow.com/a/64805500/4288232
    painter.setBrush(backgroundTop);
    painter.setBrushOrigin(0, 0);
    painter.drawRect(QRect(0, 0, this->width(), 16));

    painter.setBrush(backgroundBottom);
    painter.setBrushOrigin(0, this->height()-16);
    painter.drawRect(QRect(0, this->height()-16, this->width(), 16));

    painter.restore();
}

void EnvironmentArea::mousePressEvent(QMouseEvent * event)
{
    if(!env->options.bMouse) return;

    int x = event->x();
    int y = event->y();
    Biot *pBiot = this->env->FindBiotByPoint(x, y);
    Biot *selectedBiot = this->env->GetSelectedBiot();

    if(currentTool == "cure-sicken")
    {
        if(pBiot == nullptr) return;
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
        if(pBiot == nullptr) return;
        this->env->SetSelectedBiot(pBiot->m_Id);
    }
    else if (currentTool == "feed")
    {
        if(pBiot == nullptr) return;
        //PlayResource("PL.Feed");
        pBiot->energy += pBiot->childBaseEnergy;
        pBiot->newType = GREEN_LEAF;
    }
    else if (currentTool == "mutate")
    {
        if(pBiot == nullptr) return;
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

        try {

            ParseResult ok = d.ParseStream(isw);
            if (!ok)
                throw runtime_error("eror parsing json");
            pBiot = new Biot(*env);
            if (!d.IsObject() or !d.HasMember("biot"))
                throw runtime_error("eror parsing json");
            pBiot->SerializeJsonLoad(d["biot"]);

        }
        catch (exception &err) {

            std::cout << err.what() << std::endl;
            return;
        }

        pBiot->Place(x, y);
        pBiot->OnOpen();
        env->AddBiot(pBiot);
    }
    else if (currentTool == "relocate")
    {
        if(!selectedBiot)
        {
            if(pBiot != nullptr)
                this->env->SetSelectedBiot(pBiot->m_Id);
        }
        else
        {
            selectedBiot->Place(x, y);
            this->env->SetSelectedBiot(0);
        }
    }
    else if (currentTool == "save")
    {
        if(pBiot == nullptr) return;
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
        if(pBiot == nullptr) return;
        //PlayResource("PL.Terminate");
        pBiot->newType = YELLOW_LEAF;
        pBiot->m_age = pBiot->m_maxAge;
    }

}

void EnvironmentArea::mouseReleaseEvent(QMouseEvent * event)
{
    if(!env->options.bMouse) return;

    int x = event->x();
    int y = event->y();

}

void EnvironmentArea::setCurrentTool(const std::string &tool)
{
    currentTool = tool;
}
