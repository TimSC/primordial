#include "networking.h"
#include <QIODevice>
#include <QtEndian>
#include <QDateTime>
#include <iostream>
#include <sstream>
#include "core/json.h"
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include "core/Biots.h"
using namespace std;
using namespace rapidjson;

const uint32_t MAX_PAGE_SIZE = 1024*1024;

std::string SocketStateToString(QAbstractSocket::SocketState state)
{
    if(state==QAbstractSocket::UnconnectedState) return "Unconnected";
    if(state==QAbstractSocket::HostLookupState) return "Host Lookup";
    if(state==QAbstractSocket::ConnectingState) return "Connecting";
    if(state==QAbstractSocket::ConnectedState) return "Connected";
    if(state==QAbstractSocket::BoundState) return "Bound";
    if(state==QAbstractSocket::ClosingState) return "Closing";
    if(state==QAbstractSocket::ListeningState) return "Listening";
    return "Unknown";
}

// *******************************

Networking::Networking()
{
    connect(this, &QTcpServer::newConnection, this, &Networking::acceptConnection);
    magicCode = "primlife";
    magicCodeLen = magicCode.size();
}

Networking::~Networking()
{

}

void Networking::connectToHost(QTcpSocket *socket, const QString &hostName, quint16 port)
{
    connect(socket, &QTcpSocket::readyRead, this, &Networking::clientBytesAvailable);
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(clientStateChanged(QAbstractSocket::SocketState)));
    socket->connectToHost(hostName, port);
}

void Networking::acceptConnection()
{
    QTcpSocket *client = nextPendingConnection();
    while(client != nullptr)
    {
        std::cout << "acceptConnection" << std::endl;

        connect(client, &QTcpSocket::readyRead, this, &Networking::clientBytesAvailable);
        connect(client, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(clientStateChanged(QAbstractSocket::SocketState)));

        clients.append(client);
        assembleBuffers[client] = QByteArray();
        emit netAcceptConnection(client);
        emit netStateChanged(client, client->state());

        client = nextPendingConnection();
    }
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

void Networking::clientStateChanged(QAbstractSocket::SocketState state)
{
    cout << SocketStateToString(state) << endl;
    QTcpSocket *client = qobject_cast<QTcpSocket *>(QObject::sender());
    if(client == nullptr) return;

    if(state==QAbstractSocket::UnconnectedState)
    {
        std::cout << "disconnected " << (uint64_t)client << std::endl;
        clients.removeAll(client);


    }
    else if(state==QAbstractSocket::ConnectedState)
    {
        std::cout << "connected " << (uint64_t)client << std::endl;
        clients.append(client);
        assembleBuffers[client] = QByteArray();
    }

    emit netStateChanged(client, state);
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

void SidesManagerEventRx::ReadyToReceive(int sideId, bool ready)
{
    manager->readyToReceive(sideId, ready);
}

// ***************

SidesManager::SidesManager(class Environment &envIn) :
    QObject(), env(envIn), eventRx(this)
{
    for(int i=0;i<4; i++)
    {
        sockets[i] = nullptr;
        status[i] = "no connection";
        isAssigned[i] = false;
    }

    connect(&networking, SIGNAL(netAcceptConnection(QTcpSocket *)), this, SLOT(netAcceptConnection(QTcpSocket *)));
    connect(&networking, SIGNAL(netStateChanged(QTcpSocket *, QAbstractSocket::SocketState)), this, SLOT(netStateChanged(QTcpSocket *, QAbstractSocket::SocketState)));
    connect(&networking, SIGNAL(netReceivedPage(QTcpSocket *, const char *, uint32_t)), this, SLOT(netReceivedPage(QTcpSocket *, const char *, uint32_t)));

    updateListenMode();

    envIn.side[0]->SetListener(&eventRx);
    envIn.side[1]->SetListener(&eventRx);
    envIn.side[2]->SetListener(&eventRx);
    envIn.side[3]->SetListener(&eventRx);
}

SidesManager::~SidesManager()
{

}

bool SidesManager::isListening(uint16_t &portOut)
{
    portOut = networking.serverPort();
    return networking.isListening();
}

void SidesManager::connectToHost(int side, const QString &hostName, quint16 port)
{
    if(sockets[side] != nullptr)
        sockets[side]->disconnectFromHost();

    sockets[side] = new QTcpSocket(this);
    networking.connectToHost(sockets[side], hostName, port);
}

void SidesManager::disconnectSide(int side)
{
    if(sockets[side] != nullptr)
        sockets[side]->disconnectFromHost();
}

void SidesManager::netAcceptConnection(QTcpSocket *client)
{
    //Assign a side if possible
    int freeSide = -1;
    for(int i=0;i<4; i++)
        if(sockets[i] == nullptr)
        {
            freeSide = i;
            break;
        }

    if(freeSide >= 0)
    {
        sockets[freeSide] = client;
        isAssigned[freeSide] = true;
        QByteArray data("assignside{}");
        networking.sendPage(client, data.constData(), data.length());
        env.side[freeSide]->SetConnected(true);
        env.side[freeSide]->SetRemoteReady(true);
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

void SidesManager::netStateChanged(QTcpSocket *client, QAbstractSocket::SocketState state)
{
    int sideId = -1;
    for(int i=0;i<4; i++)
        if(sockets[i] == client)
        {
            status[i] = SocketStateToString(state).c_str();
            sideId = i;
            break;
        }

    if(state==QAbstractSocket::UnconnectedState)
    {
        if(sideId >= 0)
        {
            sockets[sideId] = nullptr;
            isAssigned[sideId] = false;
            env.side[sideId]->SetConnected(false);
            env.side[sideId]->Clear(&this->env);
            env.side[sideId]->SetSize(false);
        }
    }
    else if(state==QAbstractSocket::ConnectedState)
    {

    }

    if(sideId >= 0)
        emit sideStateChanged(sideId, state);

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
    if(rpcType == "transfbiot" or rpcType == "transbiotc")
    {
        receiveBiotFromNetwork(rpcType, side, d);
    }
    else if(rpcType == "assignside")
    {
        isAssigned[side] = true;
        env.side[side]->SetConnected(true);
        env.side[side]->SetRemoteReady(true);
        env.side[side]->Clear(&this->env);
        env.side[side]->SetSize(true);
        emit sideAssigned(side);
    }
    else if(rpcType == "nofreeside")
    {
        client->disconnectFromHost();
    }
    else if(rpcType == "sidunready")
    {
        env.side[side]->SetRemoteReady(false);
    }
    else if(rpcType == "sidereadyy")
    {
        env.side[side]->SetRemoteReady(true);
    }
}

void SidesManager::getSideStatus(int side, QString &hostPortOut, QString &statusOut, bool &enableConnect)
{
    assert(side >= 0 and side <= 4);
    hostPortOut = "";
    statusOut = "";
    enableConnect = true;
    if(sockets[side] != nullptr)
    {
        QTcpSocket *sock = sockets[side];
        hostPortOut = QString::asprintf("%s:%d", sock->peerName().toStdString().c_str(), sock->peerPort());
        QTcpSocket::SocketState state = sock->state();
        statusOut = SocketStateToString(state).c_str();
        if(state == QTcpSocket::SocketState::HostLookupState || state == QTcpSocket::SocketState::ConnectingState || state == QTcpSocket::SocketState::ConnectedState)
            enableConnect = false;
    }
    if(isAssigned[side])
        statusOut = "Assigned";
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
        if(1)
        {
            //Sent biot in compressed json
            QByteArray data("transbiotc");
            QByteArray dat1(qCompress(serBiot.c_str(), serBiot.size()));
            data.append(dat1);
            networking.sendPage(sock, data.constData(), data.length());
        }
        else
        {
            //Sent biot in uncompressed json
            QByteArray data("transfbiot");
            data.append(serBiot.c_str(), serBiot.size());
            networking.sendPage(sock, data.constData(), data.length());
        }
    }
}

void SidesManager::receiveBiotFromNetwork(const QString &rpcType, int side, QByteArray &d)
{
    cout << "biot arriving " << side << endl;

    QSharedPointer<IStreamWrapper> isw;
    QSharedPointer<stringstream> ss;
    QSharedPointer<ifstream> ifs;
    if(rpcType == "transfbiot")
    {
        ss = QSharedPointer<stringstream>(new stringstream(d.mid(10).constData()));
        isw = QSharedPointer<IStreamWrapper>(new IStreamWrapper(*ss.data()));
    }
    else if(rpcType == "transbiotc")
    {
        QByteArray dat(qUncompress(d.mid(10)));

        ss = QSharedPointer<stringstream>(new stringstream(dat.constData()));
        isw = QSharedPointer<IStreamWrapper>(new IStreamWrapper(*ss.data()));
    }
    else return;

    Biot *pBiot = nullptr;
    try {

        Document doc;
        ParseResult ok = doc.ParseStream(*isw.data());
        if (!ok)
            throw runtime_error("eror parsing json");
        pBiot = new Biot(env);
        if (!doc.IsObject() or !doc.HasMember("biot"))
            throw runtime_error("eror parsing json");
        pBiot->SerializeJsonLoad(doc["biot"]);

    } catch (exception &err) {

        std::cout << err.what() << std::endl;
        return;
    }

    pBiot->OnOpen();
    env.side[side]->ReceiveBiotFromNetwork(pBiot);
}

void SidesManager::readyToReceive(int sideId, bool ready)
{
    QTcpSocket *sock = sockets[sideId];
    if(sock)
    {
        if(ready)
        {
            QByteArray data("sidereadyy{}");
            networking.sendPage(sock, data.constData(), data.length());
        }
        else
        {
            QByteArray data("sidunready{}");
            networking.sendPage(sock, data.constData(), data.length());
        }
    }
}

void SidesManager::updateListenMode()
{
    if(env.options.m_enableNetworking and !networking.isListening())
    {
        networking.listen(QHostAddress::Any, env.options.m_networkPort);
        if(networking.isListening())
            std::cout << "listening on port " << networking.serverPort() << std::endl;
        else
        {
            std::cout << "failed to listen on port " << env.options.m_networkPort << std::endl;

            networking.listen(QHostAddress::Any);
            if(networking.isListening())
                std::cout << "listening on fall back port " << networking.serverPort() << std::endl;
            else
                std::cout << "failed to listen on fall back  port" << std::endl;
        }
    }
    if(!env.options.m_enableNetworking and networking.isListening())
    {
        std::cout << "stopping network listen" << std::endl;
        networking.close();
    }
}

// ************************************************

AutoConnect::AutoConnect(class Environment &envIn, class SidesManager &sideManagerIn):
    env(envIn),
    sideManager(sideManagerIn)
{
    previousActionTime = 0;
}

AutoConnect::~AutoConnect()
{

}

void AutoConnect::TimedUpdate()
{
    int64_t now = QDateTime::currentMSecsSinceEpoch();
    int64_t elapse = now - previousActionTime;
    if(elapse > 1000.0)
    {
        previousActionTime = now;

        std::cout << "tick" << std::endl;
    }

}
