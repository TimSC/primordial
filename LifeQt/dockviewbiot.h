#ifndef DOCKVIEWBIOT_H
#define DOCKVIEWBIOT_H

#include <QWidget>
#include "core/Environ.h"

namespace Ui {
class DockViewBiot;
}

class EnvironmentListenerToDockViewBiot : public EnvironmentListener
{
public:
    EnvironmentListenerToDockViewBiot();
    virtual ~EnvironmentListenerToDockViewBiot();

    virtual void BiotUpdated(Biot* pBiot);

    class DockViewBiot *dockViewBiot;

};

class DockViewBiot : public QWidget
{
    Q_OBJECT

public:
    explicit DockViewBiot(QWidget *parent = nullptr);
    ~DockViewBiot();

    void BiotUpdated(Biot* pBiot);

    class EnvironmentListenerToDockViewBiot listenAdapter;
    class Environment *env;

public slots:

    void SelectedBiot(uint32_t biotId);

private slots:
    void on_applyButton_clicked();

private:
    Ui::DockViewBiot *ui;
    uint32_t currentBiotId;
};

#endif // DOCKVIEWBIOT_H
