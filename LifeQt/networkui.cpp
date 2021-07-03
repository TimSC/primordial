#include "networkui.h"
#include "ui_networkui.h"
#include <QUrl>

NetworkUi::NetworkUi(SidesManager &sidesManagerIn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkUi),
    sidesManager(sidesManagerIn)
{
    ui->setupUi(this);

    connect(&sidesManager, SIGNAL(sideStateChanged(int, QAbstractSocket::SocketState)), this, SLOT(sideStateChanged(int, QAbstractSocket::SocketState)));
    connect(&sidesManager, SIGNAL(sideAssigned(int)), this, SLOT(sideAssigned(int)));

    UpdateRow(0, this->ui->addressEdit, this->ui->status, this->ui->connectButton);
    UpdateRow(1, this->ui->addressEdit_2, this->ui->status_2, this->ui->connectButton_2);
    UpdateRow(2, this->ui->addressEdit_3, this->ui->status_3, this->ui->connectButton_3);
    UpdateRow(3, this->ui->addressEdit_4, this->ui->status_4, this->ui->connectButton_4);
}

NetworkUi::~NetworkUi()
{
    delete ui;
}

void NetworkUi::on_connectButton_clicked()
{
    ConnectRow(0, this->ui->connectButton, this->ui->addressEdit);
}

void NetworkUi::on_connectButton_2_clicked()
{
    ConnectRow(1, this->ui->connectButton_2, this->ui->addressEdit_2);
}

void NetworkUi::on_connectButton_3_clicked()
{
    ConnectRow(2, this->ui->connectButton_3, this->ui->addressEdit_3);
}

void NetworkUi::on_connectButton_4_clicked()
{
    ConnectRow(3, this->ui->connectButton_4, this->ui->addressEdit_4);
}

void NetworkUi::sideAssigned(int side)
{
    sideStateChanged(side, QAbstractSocket::SocketState::ConnectingState);
}

void NetworkUi::sideStateChanged(int side, QAbstractSocket::SocketState state)
{
    if(side == 0)
        UpdateRow(0, this->ui->addressEdit, this->ui->status, this->ui->connectButton);
    else if(side == 1)
        UpdateRow(1, this->ui->addressEdit_2, this->ui->status_2, this->ui->connectButton_2);
    else if(side == 2)
        UpdateRow(2, this->ui->addressEdit_3, this->ui->status_3, this->ui->connectButton_3);
    else if(side == 3)
        UpdateRow(3, this->ui->addressEdit_4, this->ui->status_4, this->ui->connectButton_4);
}

void NetworkUi::UpdateRow(int side, QLineEdit *lineEdit, QLineEdit *statusEdit, QPushButton *button)
{
    QString addr, status;
    bool enableConnect=0;
    sidesManager.getSideStatus(side, addr, status, enableConnect);
    lineEdit->setText(addr);
    statusEdit->setText(status);
    if(enableConnect)
        button->setText("Connect");
    else
        button->setText("Disconnect");
}

void NetworkUi::ConnectRow(int side, QPushButton *button, QLineEdit *lineEdit)
{
    if(button->text() == "Connect")
    {
        QString addr = lineEdit->text();
        QUrl addrParsed("tcp://"+addr);
        int port = addrParsed.port();
        if (port < 0) port = 56436;

        sidesManager.connectToHost(side, addrParsed.host(), port);
    }
    else
        sidesManager.disconnectSide(side);
}
