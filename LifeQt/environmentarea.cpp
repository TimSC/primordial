#include "environmentarea.h"
#include <QPainter>
#include <iostream>
using namespace std;

EnvironmentArea::EnvironmentArea(QWidget *central) : QOpenGLWidget(central)
{

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
    QPainter painter(this);
    painter.setPen(QPen(Qt::blue));

    painter.drawLine(QPointF(0,0), QPointF(100, 50));

    QPainterPath path;
    path.moveTo(20, 80);
    path.lineTo(20, 30);
    path.cubicTo(80, 0, 50, 50, 80, 80);

    painter.drawPath(path);
}
