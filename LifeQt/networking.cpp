#include "networking.h"
#include <QIODevice>
#include <iostream>

PrimordialServer::PrimordialServer()
{
    connect(this, &QTcpServer::newConnection, this, &PrimordialServer::acceptConnection);
}

PrimordialServer::~PrimordialServer()
{

}

void PrimordialServer::acceptConnection()
{
    QTcpSocket *client = nextPendingConnection();
    while(client != nullptr)
    {
        std::cout << "acceptConnection" << std::endl;
        connect(client, &QTcpSocket::disconnected, this, &PrimordialServer::clientDisconnected);
        connect(client, &QTcpSocket::readyRead, this, &PrimordialServer::clientBytesAvailable);

        clients.append(client);

        client = nextPendingConnection();
    }
}

void PrimordialServer::clientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(QObject::sender());
    if(client == nullptr) return;

    std::cout << "disconnected" << std::endl;
    clients.removeAll(client);
}

void PrimordialServer::clientBytesAvailable()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(QObject::sender());
    if(client == nullptr) return;

    while(client->bytesAvailable())
    {

        qint64 readBytes = client->read(rxBuffer, sizeof(rxBuffer));

        std::cout << "rx" << readBytes << std::endl;
    }
}
