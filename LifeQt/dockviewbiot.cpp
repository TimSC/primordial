#include "dockviewbiot.h"
#include "ui_dockviewbiot.h"

DockViewBiot::DockViewBiot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DockViewBiot)
{
    ui->setupUi(this);
}

DockViewBiot::~DockViewBiot()
{
    delete ui;
}
