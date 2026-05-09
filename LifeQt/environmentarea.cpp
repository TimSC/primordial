#include "environmentarea.h"
#include "core/Biots.h"
#include <QCanvasImagePattern>
#include <QCanvasLinearGradient>
#include <QCanvasPainter>
#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QMouseEvent>
#include <iostream>
#include <fstream>
#include <memory>
#include "core/json.h"
#include "rapidjson/writer.h"
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>

using namespace std;
using namespace rapidjson;

const qint64 MAX_SINGLE_BIOT_FILE_SIZE = 1024*1024;

EnvironmentArea::EnvironmentArea(QWidget *central) : QCanvasPainterWidget(central)
{
    this->env = nullptr;
    setFillColor(Qt::black);

    tickStart = QDateTime::currentMSecsSinceEpoch();
    tickCount = 0;
    ticksPerSec = 0.0;

    backgroundTopImage.load(":/res/top.png");
    backgroundBottomImage.load(":/res/bottom.png");
    currentTool = "examine";

}

void EnvironmentArea::SetEnvironment(class Environment *envIn)
{
    this->env = envIn;
}

void EnvironmentArea::resizeEvent(QResizeEvent* event)
{
    //your code...
    QCanvasPainterWidget::resizeEvent(event);
}

void EnvironmentArea::initializeResources(QCanvasPainter *painter)
{
    const auto flags = QCanvasPainter::ImageFlag::Repeat |
                       QCanvasPainter::ImageFlag::GenerateMipmaps;
    if (!backgroundTopImage.isNull())
        backgroundTop = painter->addImage(backgroundTopImage, flags);
    if (!backgroundBottomImage.isNull())
        backgroundBottom = painter->addImage(backgroundBottomImage, flags);
}

void EnvironmentArea::graphicsResourcesInvalidated()
{
    backgroundTop = {};
    backgroundBottom = {};
}

void EnvironmentArea::paint(QCanvasPainter *painter)
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

    this->paintBackground(painter);

    if (this->env)
        this->env->paint(painter);
}

void EnvironmentArea::paintBackground(QCanvasPainter *painter)
{
    painter->save();

    QCanvasLinearGradient background(0, 0, 0, this->height());
    background.setStartColor(QColor(0,0,100));
    background.setEndColor(Qt::black);
    painter->setFillStyle(background);
    painter->fillRect(this->rect());

    if (!backgroundTop.isNull()) {
        QCanvasImagePattern pattern(backgroundTop, 0, 0, backgroundTop.width(), backgroundTop.height());
        painter->setFillStyle(pattern);
        painter->fillRect(QRectF(0, 0, this->width(), 16));
    }

    if (!backgroundBottom.isNull()) {
        QCanvasImagePattern pattern(backgroundBottom, 0, this->height() - 16,
                                    backgroundBottom.width(), backgroundBottom.height());
        painter->setFillStyle(pattern);
        painter->fillRect(QRectF(0, this->height()-16, this->width(), 16));
    }

    painter->restore();
}

void EnvironmentArea::mousePressEvent(QMouseEvent * event)
{
    if(window()->isFullScreen())
    {
        emit ExitFullscreen();
        return;
    }

    if(!env->settings.bMouse) return;

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

        emit SelectedBiot(pBiot->m_Id);
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

        std::unique_ptr<Biot> loadedBiot;

        try {
            QFileInfo fi(fileName);
            if(fi.size() > MAX_SINGLE_BIOT_FILE_SIZE)
                throw runtime_error("file is too large");

            Document d;

            ifstream ifs(fileName.toStdString().c_str());
            IStreamWrapper isw(ifs);

            ParseResult ok = d.ParseStream(isw);
            if (!ok)
                throw runtime_error("eror parsing json");
            if (!d.IsObject() or !d.HasMember("biot"))
                throw runtime_error("eror parsing json");
            loadedBiot.reset(new Biot(*env));
            loadedBiot->SerializeJsonLoad(d["biot"]);

        }
        catch (exception &err) {

            std::cout << err.what() << std::endl;
            return;
        }

        pBiot = loadedBiot.release();
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
    (void)event;
    if(!env->settings.bMouse) return;

}

void EnvironmentArea::setCurrentTool(const std::string &tool)
{
    currentTool = tool;
}
