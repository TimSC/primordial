#include "networking.h"
#include <QIODevice>
#include <QtEndian>
#include <iostream>
#include <sstream>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include "core/Biots.h"
using namespace std;
using namespace rapidjson;

const uint32_t MAX_PAGE_SIZE = 1024*1024;

Networking::Networking()
{
    connect(this, &QTcpServer::newConnection, this, &Networking::acceptConnection);
    magicCode = "primlife";
    magicCodeLen = magicCode.size();
}

Networking::~Networking()
{

}

QTcpSocket *Networking::connectToHost(const QString &hostName, quint16 port)
{
    QTcpSocket *out = new QTcpSocket(this);
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
    if(size > MAX_PAGE_SIZE)
        throw invalid_argument("Page too large");

    client->write(magicCode.c_str(), magicCodeLen);
    uint32_t pageSize = qToBigEndian<uint32_t>(size);
    client->write((const char *)&pageSize, sizeof(uint32_t));
    client->write(data, size);

    uint32_t expectSize=qFromBigEndian<uint32_t>(pageSize);
    //cout<< "tx " << size << "," << pageSize << "," << expectSize << endl;
    assert(size == expectSize);
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

        //std::cout << "rx0 " << readBytes << " " << (uint64_t)client << std::endl;

        QByteArray &assemblyBuffer = assembleBuffers[client];

        assemblyBuffer.append(rxBuffer, readBytes);

        if(assemblyBuffer.size() >= sizeof(uint32_t)+magicCodeLen)
        {
            QByteArray chkMagicCode = assemblyBuffer.left(magicCodeLen);
            if(chkMagicCode != magicCode.c_str())
                cout << "Error in page magic code" << endl;

            uint32_t expectSize=qFromBigEndian<uint32_t>(&assemblyBuffer.constData()[magicCodeLen]);
            if(expectSize > MAX_PAGE_SIZE)
            {
                //Prevent a client using all our memory
                client->disconnectFromHost();
            }
            //std::cout << "rx1 " << expectSize << " " << *(uint32_t *)&assemblyBuffer.constData()[magicCodeLen] << " " << assemblyBuffer.size() << std::endl;

            int entirePageSize = magicCodeLen + sizeof(uint32_t) + expectSize;
            if(assemblyBuffer.size() >= entirePageSize)
            {
                pageComplete(client, &assemblyBuffer.constData()[magicCodeLen+sizeof(uint32_t)], expectSize);
                QByteArray remains = assemblyBuffer.mid(entirePageSize);
                assembleBuffers[client] = remains;
            }
        }
    }
}

void Networking::pageComplete(QTcpSocket *client, const char *data, uint32_t size)
{
    if(0)
    {
        //Fuzz the input for testing
        QByteArray tmp(data, size);
        for(int i=0; i<rand() % 3; i++)
        {
            char *data2 = &tmp.data()[rand() % size];
            *data2 = rand() % 256;
        }
        emit netReceivedPage(client, tmp.data(), size);
    }
    else
    {
        emit netReceivedPage(client, data, size);
    }
}

// ***************

SidesManagerEventRx::SidesManagerEventRx(class SidesManager *managerIn):
    SideListener(),
    manager(managerIn)
{


}

SidesManagerEventRx::~SidesManagerEventRx()
{

}

void SidesManagerEventRx::BiotLeavingSide(int side, Biot *pBiot)
{
    manager->biotLeavingSide(side, pBiot);
}

// ***************

SidesManager::SidesManager(class Environment &envIn) :
    QObject(), env(envIn), eventRx(this)
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

    envIn.side[0]->SetListener(&eventRx);
    envIn.side[1]->SetListener(&eventRx);
    envIn.side[2]->SetListener(&eventRx);
    envIn.side[3]->SetListener(&eventRx);
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
    //std::cout << freeSide << std::endl;

    if(freeSide >= 0)
    {
        sockets[freeSide] = client;
        status[freeSide] = "assigned";
        QByteArray data("assignside{}");
        networking.sendPage(client, data.constData(), data.length());
        env.side[freeSide]->SetConnected(true);
        env.side[freeSide]->Clear(&this->env);
        env.side[freeSide]->SetSize(true);

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
            env.side[i]->SetConnected(false);
            env.side[i]->Clear(&this->env);
            env.side[i]->SetSize(false);
            emit sideDisconnected(i);
        }
}

void SidesManager::netReceivedPage(QTcpSocket *client, const char *data, uint32_t size)
{
    QByteArray d(data, size);

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
        env.side[side]->SetConnected(true);
        env.side[side]->Clear(&this->env);
        env.side[side]->SetSize(true);
        emit sideAssigned(side);
    }
    else if(rpcType == "nofreeside")
    {
        client->disconnectFromHost();
    }
    else if(rpcType == "transfbiot")
    {
        cout << "biot arriving " << side << endl;

        Document doc;
        stringstream ss(d.mid(10).constData());
        IStreamWrapper isw(ss);

        doc.ParseStream(isw);
        Biot *pBiot = new Biot(env);
        if (!doc.HasMember("biot"))
            throw runtime_error("eror parsing json");
        pBiot->SerializeJsonLoad(doc["biot"]);
        pBiot->OnOpen();
        env.side[side]->ReceiveBiotFromNetwork(pBiot);
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

void SidesManager::biotLeavingSide(int side, Biot *pBiot)
{
    cout << "biotLeavingSide " << side << endl;
    //Serialize biot
    Document d;
    d.SetObject();
    Value biotJson(kObjectType);
    pBiot->SerializeJson(d, biotJson);
    d.AddMember("biot", biotJson, d.GetAllocator());
    stringstream ss;
    OStreamWrapper osw(ss);
    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);

    string serBiot = ss.str();

    //Sent via socket
    if(sockets[side] != nullptr)
    {
        QTcpSocket *sock = sockets[side];
        QByteArray data("transfbiot");
        data.append(serBiot.c_str(), serBiot.size());
        networking.sendPage(sock, data.constData(), data.length());
    }
}
