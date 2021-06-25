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

private:
    class Environment *env;
};

#endif // ENVIRONMENTAREA_H
