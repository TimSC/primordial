#ifndef ENVIRONMENTAREA_H
#define ENVIRONMENTAREA_H

#include <QCanvasImage>
#include <QCanvasPainterWidget>
#include <QImage>
#include "core/Environ.h"

class EnvironmentArea : public QCanvasPainterWidget
{
    Q_OBJECT
public:
    EnvironmentArea(QWidget *central);

    void SetEnvironment(class Environment *envIn);

    void resizeEvent(QResizeEvent* event) override;

    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void setCurrentTool(const std::string &tool);

protected:
    void initializeResources(QCanvasPainter *painter) override;
    void graphicsResourcesInvalidated() override;
    void paint(QCanvasPainter *painter) override;

signals:
    void SelectedBiot(uint32_t biotId);
    void ExitFullscreen();

private:
    class Environment *env;

    uint64_t tickStart, tickCount;
    double ticksPerSec;
    std::string currentTool;
    QImage backgroundTopImage, backgroundBottomImage;
    QCanvasImage backgroundTop, backgroundBottom;

    void paintBackground(QCanvasPainter *painter);


};

#endif // ENVIRONMENTAREA_H
