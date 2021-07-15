#ifndef DOCKVIEWBIOT_H
#define DOCKVIEWBIOT_H

#include <QWidget>

namespace Ui {
class DockViewBiot;
}

class DockViewBiot : public QWidget
{
    Q_OBJECT

public:
    explicit DockViewBiot(QWidget *parent = nullptr);
    ~DockViewBiot();

private:
    Ui::DockViewBiot *ui;
};

#endif // DOCKVIEWBIOT_H
