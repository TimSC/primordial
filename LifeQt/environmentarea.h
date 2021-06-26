#ifndef ENVIRONMENTAREA_H
#define ENVIRONMENTAREA_H

#include <QOpenGLWidget>
#include "core/Environ.h"

class EnvironmentArea : public QOpenGLWidget
{
    Q_OBJECT
public:
    EnvironmentArea(QWidget *central);

    void SetEnvironment(class Environment *envIn);

    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:
    class Environment *env;

    uint64_t tickStart, tickCount;
    double ticksPerSec;
};

#endif // ENVIRONMENTAREA_H
