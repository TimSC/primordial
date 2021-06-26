#include "environmentarea.h"
#include <QPainter>
#include <QDateTime>
#include <iostream>
using namespace std;

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



}

void EnvironmentArea::mouseReleaseEvent(QMouseEvent * event)
{
    int x = event->x();
    int y = event->y();
    std::cout << x <<"," << y << std::endl;
    int boitId = this->env->FindBiotByPoint(x, y);
    if(boitId > 0)
        this->env->SetSelectedBiot(boitId);


}
