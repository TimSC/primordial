#ifndef ENVIRONMENTAREA_H
#define ENVIRONMENTAREA_H

#include <QOpenGLWidget>

class EnvironmentArea : public QOpenGLWidget
{
    Q_OBJECT
public:
    EnvironmentArea(QWidget *central);

    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintGL() override;
};

#endif // ENVIRONMENTAREA_H
