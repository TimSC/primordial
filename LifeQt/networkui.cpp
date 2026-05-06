#include "networkui.h"
#include "ui_networkui.h"
#include <QHostAddress>
#include <QUrl>

static bool ParsePort(const QString &text, quint16 &port)
{
    bool ok = false;
    uint parsed = text.toUInt(&ok);
    if(!ok || parsed > 65535)
        return false;

    port = (quint16)parsed;
    return true;
}

static bool ParseEndpoint(const QString &input, quint16 defaultPort, QString &host, quint16 &port)
{
    QString text = input.trimmed();
    port = defaultPort;

    if(text.isEmpty())
        return false;

    if(text.startsWith("tcp://", Qt::CaseInsensitive))
    {
        QUrl url(text);
        host = url.host();
        port = (quint16)url.port(defaultPort);
        return !host.isEmpty();
    }

    if(text.startsWith("["))
    {
        int closeBracket = text.indexOf("]");
        if(closeBracket < 0)
            return false;

        host = text.mid(1, closeBracket - 1);
        QString rest = text.mid(closeBracket + 1);
        if(rest.startsWith(":") && !ParsePort(rest.mid(1), port))
            return false;

        return !host.isEmpty();
    }

    QHostAddress address;
    if(address.setAddress(text))
    {
        host = address.toString();
        return true;
    }

    if(text.count(":") == 1)
    {
        int colon = text.lastIndexOf(":");
        host = text.left(colon);
        if(!ParsePort(text.mid(colon + 1), port))
            return false;
        return !host.isEmpty();
    }

    host = text;
    return true;
}

NetworkUi::NetworkUi(SidesManager &sidesManagerIn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkUi),
    sidesManager(sidesManagerIn)
{
    ui->setupUi(this);
    ui->autoReconnectCheckBox->setChecked(sidesManager.autoReconnectEnabled());

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

void NetworkUi::on_autoReconnectCheckBox_toggled(bool checked)
{
    sidesManager.setAutoReconnectEnabled(checked);
}

void NetworkUi::sideAssigned(int side)
{
    sideStateChanged(side, QAbstractSocket::SocketState::ConnectingState);
}

void NetworkUi::sideStateChanged(int side, QAbstractSocket::SocketState state)
{
    (void)state;
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
        QString host;
        quint16 port;
        if(!ParseEndpoint(lineEdit->text(), sidesManager.defaultNetworkPort(), host, port))
            return;

        sidesManager.connectToHost(side, host, port);
    }
    else
        sidesManager.disconnectSide(side);
}
