#include "networking.h"
#include <QIODevice>
#include <QtEndian>
#include <iostream>
using namespace std;

Networking::Networking()
{
    connect(this, &QTcpServer::newConnection, this, &Networking::acceptConnection);
}

Networking::~Networking()
{

}

QTcpSocket *Networking::connectToHost(const QString &hostName, quint16 port)
{
    QTcpSocket *out = new QTcpSocket(this);
    cout << (uint64_t)out << endl;
    connect(out, &QTcpSocket::connected, this, &Networking::connected);
    connect(out, &QTcpSocket::disconnected, this, &Networking::clientDisconnected);
    connect(out, &QTcpSocket::readyRead, this, &Networking::clientBytesAvailable);
    out->connectToHost(hostName, port);

    return out;
}

void Networking::acceptConnection()
{
    QTcpSocket *client = nextPendingConnection();
    while(client != nullptr)
    {
        std::cout << "acceptConnection" << std::endl;
        connect(client, &QTcpSocket::disconnected, this, &Networking::clientDisconnected);
        connect(client, &QTcpSocket::readyRead, this, &Networking::clientBytesAvailable);

        clients.append(client);
        assembleBuffers[client] = QByteArray();

        emit netConnected(client);

        client = nextPendingConnection();
    }


}

void Networking::connected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(QObject::sender());
    if(client == nullptr) return;

    std::cout << "connected " << (uint64_t)client << std::endl;
    clients.append(client);
    assembleBuffers[client] = QByteArray();

    emit netConnected(client);
}

void Networking::sendPage(QTcpSocket *client, const char *data, uint32_t size)
{
    uint32_t pageSize=0;
    qToBigEndian<quint32>(size, &pageSize);
    client->write((const char *)&pageSize, sizeof(uint32_t));
    client->write(data, size);
}

void Networking::clientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(QObject::sender());
    if(client == nullptr) return;

    std::cout << "disconnected " << (uint64_t)client << std::endl;
    clients.removeAll(client);

    emit netDisconnected(client);
}

void Networking::clientBytesAvailable()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(QObject::sender());
    if(client == nullptr) return;

    while(client->bytesAvailable())
    {

        qint64 readBytes = client->read(rxBuffer, sizeof(rxBuffer));

        std::cout << "rx" << readBytes << " " << (uint64_t)client << std::endl;

        QByteArray &assemblyBuffer = assembleBuffers[client];
        assemblyBuffer.append(rxBuffer, readBytes);

        if(assemblyBuffer.size() >= sizeof(uint32_t))
        {
            uint32_t expectSize=qFromBigEndian<uint32_t>(assemblyBuffer.constData());
            if(assemblyBuffer.size() >= sizeof(uint32_t) + expectSize)
            {
                pageComplete(client, &assemblyBuffer.constData()[sizeof(uint32_t)], expectSize);
                QByteArray remains(&assemblyBuffer.constData()[sizeof(uint32_t) + expectSize]);
                assemblyBuffer = remains;
            }
        }
    }
}

void Networking::pageComplete(QTcpSocket *client, const char *data, uint32_t size)
{
    emit netReceivedPage(client, data, size);
}

// ***************

SidesManager::SidesManager() : QObject()
{
    for(int i=0;i<4; i++)
    {
        sockets[i] = nullptr;
        status[i] = "no connection";
    }

    connect(&networking, SIGNAL(netConnected(QTcpSocket *)), this, SLOT(netConnected(QTcpSocket *)));
    connect(&networking, SIGNAL(netDisconnected(QTcpSocket *)), this, SLOT(netDisconnected(QTcpSocket *)));
    connect(&networking, SIGNAL(netReceivedPage(QTcpSocket *, const char *, uint32_t)), this, SLOT(netReceivedPage(QTcpSocket *, const char *, uint32_t)));

    networking.listen(QHostAddress::Any);
    std::cout << "listening on port " << networking.serverPort() << std::endl;
}

SidesManager::~SidesManager()
{

}

void SidesManager::connectToHost(int side, const QString &hostName, quint16 port)
{
    if(sockets[side] != nullptr)
        sockets[side]->disconnectFromHost();

    QTcpSocket *socket = networking.connectToHost(hostName, port);
    sockets[side] = socket;
    status[side] = "connecting";
    emit sideConnecting(side);
}

void SidesManager::disconnectSide(int side)
{
    if(sockets[side] != nullptr)
        sockets[side]->disconnectFromHost();
}

void SidesManager::netConnected(QTcpSocket *client)
{
    //Check if already known
    for(int i=0;i<4; i++)
        if(sockets[i] == client)
        {
            status[i] = "connected";
            emit sideConnected(i);
            return;
        }

    //Assign a side if possible
    int freeSide = -1;
    for(int i=0;i<4; i++)
        if(sockets[i] == nullptr)
        {
            freeSide = i;
            break;
        }
    std::cout << freeSide << std::endl;

    if(freeSide >= 0)
    {
        sockets[freeSide] = client;
        status[freeSide] = "assigned";
        QByteArray data("assignside{}");
        networking.sendPage(client, data.constData(), data.length());

        emit sideAssigned(freeSide);
    }
    else
    {
        QByteArray data("nofreeside{}");
        networking.sendPage(client, data.constData(), data.length());
        //client->disconnectFromHost();
    }
}

void SidesManager::netDisconnected(QTcpSocket *client)
{
    for(int i=0;i<4; i++)
        if(sockets[i] == client)
        {
            status[i] = "disconnected";
            sockets[i] = nullptr;
            emit sideDisconnected(i);
        }
}

void SidesManager::netReceivedPage(QTcpSocket *client, const char *data, uint32_t size)
{
    QByteArray d(data, size);
    std::cout << "page" << d.constData() << std::endl;

    int side = -1;
    for(int i=0;i<4; i++)
    {
        if(sockets[i] == client)
        {
            side = i;
            break;
        }
    }
    if(side == -1) return;

    QString rpcType = d.left(10);
    if(rpcType == "assignside")
    {
        status[side] = "assigned";
        emit sideAssigned(side);
    }
    else if(rpcType == "nofreeside")
    {
        client->disconnectFromHost();
    }
}

void SidesManager::getSideStatus(int side, QString &hostPortOut, QString &statusOut, bool &enableConnect)
{
    assert(side >= 0 and side <= 4);
    if(sockets[side] != nullptr)
    {
        QTcpSocket *sock = sockets[side];
        hostPortOut = QString::asprintf("%s:%d", sock->peerName().toStdString().c_str(), sock->peerPort());
    }
    else
        hostPortOut = "";
    statusOut = status[side];
    enableConnect = true;
    if(statusOut == "connecting" or statusOut == "connected" or statusOut == "assigned")
        enableConnect = false;
}
