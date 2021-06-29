#include "networkui.h"
#include "ui_networkui.h"
#include <QUrl>

NetworkUi::NetworkUi(SidesManager &sidesManagerIn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkUi),
    sidesManager(sidesManagerIn)
{
    ui->setupUi(this);

    connect(&sidesManager, SIGNAL(sideConnected(int)), this, SLOT(sideChangedStatus(int)));
    connect(&sidesManager, SIGNAL(sideAssigned(int)), this, SLOT(sideChangedStatus(int)));
    connect(&sidesManager, SIGNAL(sideDisconnected(int)), this, SLOT(sideChangedStatus(int)));

    QString addr, status;
    sidesManager.getSideStatus(0, addr, status);
    this->ui->addressEdit->setText(addr);
    this->ui->status->setText(status);

    sidesManager.getSideStatus(1, addr, status);
    this->ui->addressEdit_2->setText(addr);
    this->ui->status_2->setText(status);

    sidesManager.getSideStatus(2, addr, status);
    this->ui->addressEdit_3->setText(addr);
    this->ui->status_3->setText(status);

    sidesManager.getSideStatus(3, addr, status);
    this->ui->addressEdit_4->setText(addr);
    this->ui->status_4->setText(status);
}

NetworkUi::~NetworkUi()
{
    delete ui;
}

void NetworkUi::on_connectButton_clicked()
{
    QString addr = this->ui->addressEdit->text();
    QUrl addrParsed("tcp://"+addr);
    int port = addrParsed.port();
    if (port < 0) port = 56436;

    sidesManager.connectToHost(0, addrParsed.host(), port);
}

void NetworkUi::on_connectButton_2_clicked()
{
    QString addr = this->ui->addressEdit_2->text();
    QUrl addrParsed("tcp://"+addr);
    int port = addrParsed.port();
    if (port < 0) port = 56436;

    sidesManager.connectToHost(1, addrParsed.host(), port);
}

void NetworkUi::on_connectButton_3_clicked()
{
    QString addr = this->ui->addressEdit_3->text();
    QUrl addrParsed("tcp://"+addr);
    int port = addrParsed.port();
    if (port < 0) port = 56436;

    sidesManager.connectToHost(2, addrParsed.host(), port);
}

void NetworkUi::on_connectButton_4_clicked()
{
    QString addr = this->ui->addressEdit_4->text();
    QUrl addrParsed("tcp://"+addr);
    int port = addrParsed.port();
    if (port < 0) port = 56436;

    sidesManager.connectToHost(3, addrParsed.host(), port);
}

void NetworkUi::sideChangedStatus(int side)
{
    QString addr, status;
    if(side == 0)
    {
        sidesManager.getSideStatus(0, addr, status);
        this->ui->addressEdit->setText(addr);
        this->ui->status->setText(status);
    }
    else if(side == 1)
    {
        sidesManager.getSideStatus(1, addr, status);
        this->ui->addressEdit_2->setText(addr);
        this->ui->status_2->setText(status);
    }
    else if(side == 2)
    {
        sidesManager.getSideStatus(2, addr, status);
        this->ui->addressEdit_3->setText(addr);
        this->ui->status_3->setText(status);
    }
    else if(side == 3)
    {
        sidesManager.getSideStatus(3, addr, status);
        this->ui->addressEdit_4->setText(addr);
        this->ui->status_4->setText(status);
    }
}
