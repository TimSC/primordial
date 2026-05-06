#include "networking.h"
#include <QIODevice>
#include <QtEndian>
#include <QDateTime>
#include <QUrl>
#include <iostream>
#include <memory>
#include <sstream>
#include "core/json.h"
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include "core/Biots.h"
using namespace std;
using namespace rapidjson;

const uint32_t MAX_PAGE_SIZE = 1024*1024;
const uint32_t MAX_BIOT_JSON_SIZE = 512*1024;
const qint64 RETURN_BIOT_WINDOW_MS = 10000;
const qint64 RECONNECT_INTERVAL_MS = 60000;
const char *RPC_PEER_HELLO = "peerhello1";

static uint32_t GetQtCompressedSize(const QByteArray &data)
{
    if(data.size() < (int)sizeof(uint32_t))
        throw runtime_error("compressed biot payload is truncated");

    return qFromBigEndian<uint32_t>((const uchar *)data.constData());
}

static QByteArray UncompressBiotPayload(const QByteArray &data)
{
    uint32_t uncompressedSize = GetQtCompressedSize(data);
    if(uncompressedSize > MAX_BIOT_JSON_SIZE)
        throw runtime_error("compressed biot payload is too large");

    QByteArray uncompressed = qUncompress(data);
    if(uncompressed.isEmpty() || uncompressed.size() > (int)MAX_BIOT_JSON_SIZE)
        throw runtime_error("compressed biot payload is invalid");

    return uncompressed;
}

static bool RpcIsCompressedBiotPayload(const QString &rpcType)
{
    return rpcType == "transbiotc" or rpcType == "returnbioc";
}

static bool RpcIsUncompressedBiotPayload(const QString &rpcType)
{
    return rpcType == "transfbiot" or rpcType == "returnbiot";
}

static QByteArray DecodeBiotPayload(const QString &rpcType, const QByteArray &data)
{
    QByteArray payload(data.mid(10));
    if(RpcIsUncompressedBiotPayload(rpcType))
    {
        if(payload.size() > (int)MAX_BIOT_JSON_SIZE)
            throw runtime_error("biot payload is too large");
        return payload;
    }
    if(RpcIsCompressedBiotPayload(rpcType))
        return UncompressBiotPayload(payload);

    throw runtime_error("unknown biot payload type");
}

static uint32_t ExtractBiotIdFromNetworkPayload(const QString &rpcType, const QByteArray &data)
{
    QByteArray rawPayload(data.mid(10));
    if((rpcType == "returnbiot" or rpcType == "returnbioc") && rawPayload.size() == (int)sizeof(uint32_t))
        return qFromBigEndian<uint32_t>((const uchar *)rawPayload.constData());

    QByteArray payload = DecodeBiotPayload(rpcType, data);
    stringstream ss(std::string(payload.constData(), payload.size()));
    IStreamWrapper isw(ss);

    Document doc;
    ParseResult ok = doc.ParseStream(isw);
    if(!ok || !doc.IsObject() || !doc.HasMember("biot") || !doc["biot"].IsObject())
        throw runtime_error("eror parsing json");

    const Value &biot = doc["biot"];
    if(!biot.HasMember("m_Id") || !biot["m_Id"].IsUint())
        throw runtime_error("biot payload missing id");

    return biot["m_Id"].GetUint();
}

static void PruneRecentlySentBiots(QMap<uint32_t, RecentlySentBiot> &sentBiots, qint64 now)
{
    QMutableMapIterator<uint32_t, RecentlySentBiot> sentIt(sentBiots);
    while(sentIt.hasNext()) {
        sentIt.next();
        if(now - sentIt.value().sentTime > RETURN_BIOT_WINDOW_MS)
            sentIt.remove();
    }
}

static QString FormatHostPort(const QString &host, quint16 port)
{
    QHostAddress address;
    if(address.setAddress(host) && address.protocol() == QAbstractSocket::IPv6Protocol)
        return QString("[%1]:%2").arg(address.toString()).arg(port);

    if(host.contains(":") && !host.startsWith("["))
        return QString("[%1]:%2").arg(host).arg(port);

    return QString("%1:%2").arg(host).arg(port);
}

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

        if(assemblyBuffer.size() >= (int)(sizeof(uint32_t)+magicCodeLen))
        {
            QByteArray chkMagicCode = assemblyBuffer.left(magicCodeLen);
            if(chkMagicCode != magicCode.c_str())
            {
                cout << "Error in page magic code" << endl;
                assembleBuffers[client].clear();
                client->disconnectFromHost();
                return;
            }

            uint32_t expectSize=qFromBigEndian<uint32_t>(&assemblyBuffer.constData()[magicCodeLen]);
            if(expectSize > MAX_PAGE_SIZE)
            {
                //Prevent a client using all our memory
                assembleBuffers[client].clear();
                client->disconnectFromHost();
                return;
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

bool SidesManagerEventRx::BiotLeavingSide(int side, Biot *pBiot)
{
    return manager->biotLeavingSide(side, pBiot);
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
        configuredHostPort[i] = QString::fromStdString(env.settings.m_sSideAddress[i]);
        peerInstanceId[i] = "";
        recentlySentBiots[i].clear();
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
    assert(side >= 0 and side < 4);
    if(sockets[side] != nullptr)
    {
        sockets[side]->disconnectFromHost();
        sockets[side]->deleteLater();
    }

    recentlySentBiots[side].clear();
    configuredHostPort[side] = FormatHostPort(hostName, port);
    env.settings.m_sSideAddress[side] = configuredHostPort[side].toStdString();
    env.settings.m_bSideEnable[side] = true;
    env.settings.Save();

    sockets[side] = new QTcpSocket(this);
    networking.connectToHost(sockets[side], hostName, port);
}

void SidesManager::disconnectSide(int side)
{
    assert(side >= 0 and side < 4);
    env.settings.m_bSideEnable[side] = false;
    env.settings.m_sSideAddress[side] = "";
    configuredHostPort[side] = "";
    env.settings.Save();

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
            recentlySentBiots[i].clear();
            break;
        }

    if(freeSide >= 0)
    {
        sockets[freeSide] = client;
        isAssigned[freeSide] = true;
        QString peer = client->peerName();
        if(peer.isEmpty())
            peer = client->peerAddress().toString();
        configuredHostPort[freeSide] = FormatHostPort(peer, client->peerPort());
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
            QString stateStatus = SocketStateToString(state).c_str();
            if(state != QAbstractSocket::UnconnectedState ||
               (status[i] != "No free sides" && status[i] != "Loopback rejected"))
                status[i] = stateStatus;
            sideId = i;
            break;
        }

    if(state==QAbstractSocket::UnconnectedState)
    {
        if(sideId >= 0)
        {
            sockets[sideId] = nullptr;
            isAssigned[sideId] = false;
            recentlySentBiots[sideId].clear();
            peerInstanceId[sideId] = "";
            env.side[sideId]->SetConnected(false);
            env.side[sideId]->Clear(&this->env);
            env.side[sideId]->SetSize(false);
            client->deleteLater();
        }
    }
    else if(state==QAbstractSocket::ConnectedState)
    {
        sendPeerHello(client);
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
    if(rpcType == RPC_PEER_HELLO)
    {
        receivePeerHello(side, d);
    }
    else if(rpcType == "transfbiot" or rpcType == "transbiotc")
    {
        receiveBiotFromNetwork(rpcType, side, d, true);
    }
    else if(rpcType == "returnbiot" or rpcType == "returnbioc")
    {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        PruneRecentlySentBiots(recentlySentBiots[side], now);

        uint32_t biotId = 0;
        try {
            biotId = ExtractBiotIdFromNetworkPayload(rpcType, d);
        }
        catch (const exception &err) {
            std::cout << "dropping returned biot with unreadable id: " << err.what() << std::endl;
            return;
        }

        if(!recentlySentBiots[side].contains(biotId))
        {
            std::cout << "dropping unsolicited returned biot on side " << side
                      << ", id=" << biotId << std::endl;
            return;
        }
        QByteArray cachedPayload = recentlySentBiots[side][biotId].jsonPayload;
        recentlySentBiots[side].remove(biotId);
        receiveCachedReturnedBiot(side, cachedPayload);
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
        status[side] = "No free sides";
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
    assert(side >= 0 and side < 4);
    hostPortOut = "";
    statusOut = "";
    enableConnect = true;
    if(sockets[side] != nullptr)
    {
        QTcpSocket *sock = sockets[side];
        hostPortOut = configuredHostPort[side];
        if(hostPortOut.isEmpty())
        {
            QString host = sock->peerName();
            if(host.isEmpty())
                host = sock->peerAddress().toString();
            hostPortOut = FormatHostPort(host, sock->peerPort());
        }
        QTcpSocket::SocketState state = sock->state();
        statusOut = SocketStateToString(state).c_str();
        if(state == QTcpSocket::SocketState::HostLookupState || state == QTcpSocket::SocketState::ConnectingState || state == QTcpSocket::SocketState::ConnectedState)
            enableConnect = false;
    }
    else if(env.settings.m_bSideEnable[side])
    {
        hostPortOut = QString::fromStdString(env.settings.m_sSideAddress[side]);
        statusOut = status[side];
    }
    if(isAssigned[side])
        statusOut = "Assigned";
}

quint16 SidesManager::defaultNetworkPort() const
{
    return env.settings.m_networkPort;
}

void SidesManager::connectConfiguredSides()
{
    if(!env.settings.m_enableNetworking || !env.settings.m_autoReconnect)
        return;

    for(int side=0; side<4; side++)
    {
        if(sockets[side] != nullptr || !env.settings.m_bSideEnable[side])
            continue;

        QString host;
        quint16 port = defaultNetworkPort();
        QString endpoint = QString::fromStdString(env.settings.m_sSideAddress[side]);
        if(ParseEndpoint(endpoint, defaultNetworkPort(), host, port))
            connectToHost(side, host, port);
    }
}

bool SidesManager::autoReconnectEnabled() const
{
    return env.settings.m_autoReconnect;
}

void SidesManager::setAutoReconnectEnabled(bool enabled)
{
    env.settings.m_autoReconnect = enabled;
    env.settings.Save();
}

bool SidesManager::biotLeavingSide(int side, Biot *pBiot)
{
    cout << "biotLeavingSide " << side << endl;
    string serBiot;
    try {
        Document d;
        d.SetObject();
        Value biotJson(kObjectType);
        pBiot->SerializeJson(d, biotJson);
        d.AddMember("biot", biotJson, d.GetAllocator());
        stringstream ss;
        OStreamWrapper osw(ss);
        Writer<OStreamWrapper> writer(osw);
        d.Accept(writer);
        serBiot = ss.str();
    }
    catch (const exception &err) {
        std::cout << "failed to serialize outgoing biot: " << err.what() << std::endl;
        return false;
    }

    //Sent via socket
    if(sockets[side] != nullptr)
    {
        try {
            QTcpSocket *sock = sockets[side];
            qint64 now = QDateTime::currentMSecsSinceEpoch();
            PruneRecentlySentBiots(recentlySentBiots[side], now);
            if(1)
            {
                //Sent biot in compressed json
                QByteArray data("transbiotc");
                QByteArray dat1(qCompress(serBiot.c_str(), serBiot.size()));
                data.append(dat1);
                networking.sendPage(sock, data.constData(), data.length());
                recentlySentBiots[side][pBiot->m_Id] = RecentlySentBiot{now, QByteArray(serBiot.c_str(), serBiot.size())};
            }
            else
            {
                //Sent biot in uncompressed json
                QByteArray data("transfbiot");
                data.append(serBiot.c_str(), serBiot.size());
                networking.sendPage(sock, data.constData(), data.length());
                recentlySentBiots[side][pBiot->m_Id] = RecentlySentBiot{now, QByteArray(serBiot.c_str(), serBiot.size())};
            }
            return true;
        }
        catch (const exception &err) {
            std::cout << "failed to send outgoing biot: " << err.what() << std::endl;
            return false;
        }
    }
    return false;
}

void SidesManager::sendPeerHello(QTcpSocket *client)
{
    if(client == nullptr)
        return;

    Document d;
    d.SetObject();
    Document::AllocatorType& allocator = d.GetAllocator();
    QByteArray instanceId = env.settings.m_instanceId.toUtf8();
    Value instanceIdJson;
    instanceIdJson.SetString(instanceId.constData(), instanceId.size(), allocator);
    d.AddMember("instanceId", instanceIdJson, allocator);

    stringstream ss;
    OStreamWrapper osw(ss);
    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);

    string payload = ss.str();
    QByteArray data(RPC_PEER_HELLO);
    data.append(payload.c_str(), payload.size());
    networking.sendPage(client, data.constData(), data.length());
}

void SidesManager::receivePeerHello(int side, const QByteArray &d)
{
    try {
        QByteArray payload = d.mid(10);
        stringstream ss(std::string(payload.constData(), payload.size()));
        IStreamWrapper isw(ss);

        Document doc;
        ParseResult ok = doc.ParseStream(isw);
        if(!ok || !doc.IsObject() || !doc.HasMember("instanceId") || !doc["instanceId"].IsString())
            throw runtime_error("error parsing peer hello");

        QString instanceId = QString::fromUtf8(doc["instanceId"].GetString(), doc["instanceId"].GetStringLength());
        if(instanceId.isEmpty())
            throw runtime_error("peer hello missing instance id");

        peerInstanceId[side] = instanceId;
        if(instanceId == env.settings.m_instanceId)
        {
            status[side] = "Loopback rejected";
            env.settings.m_bSideEnable[side] = false;
            env.settings.Save();
            if(sockets[side] != nullptr)
                sockets[side]->disconnectFromHost();
        }
    }
    catch (const exception &err) {
        std::cout << "invalid peer hello: " << err.what() << std::endl;
    }
}

void SidesManager::returnRejectedBiotToPeer(const QString &rpcType, int side, const QByteArray &d, const char *reason)
{
    QTcpSocket *sock = sockets[side];
    if(sock == nullptr)
        return;

    uint32_t biotId = 0;
    try {
        biotId = ExtractBiotIdFromNetworkPayload(rpcType, d);
    }
    catch (const exception &err) {
        std::cout << "cannot return rejected biot without readable id: " << err.what() << std::endl;
        return;
    }

    QByteArray returned;
    if(rpcType == "transfbiot")
        returned.append("returnbiot");
    else if(rpcType == "transbiotc")
        returned.append("returnbioc");
    else
        return;

    uint32_t networkBiotId = qToBigEndian<uint32_t>(biotId);
    returned.append((const char *)&networkBiotId, sizeof(networkBiotId));
    std::cout << "returning rejected biot on side " << side << ": " << reason << std::endl;
    networking.sendPage(sock, returned.constData(), returned.length());
}

void SidesManager::receiveBiotFromNetwork(const QString &rpcType, int side, const QByteArray &d, bool returnOnFailure, bool allowQueueOverflow)
{
    cout << "biot arriving " << side << endl;

    std::unique_ptr<Biot> pBiot;
    try {
        QSharedPointer<IStreamWrapper> isw;
        QSharedPointer<stringstream> ss;
        QSharedPointer<ifstream> ifs;
        QByteArray dat = DecodeBiotPayload(rpcType, d);
        ss = QSharedPointer<stringstream>(new stringstream(std::string(dat.constData(), dat.size())));
        isw = QSharedPointer<IStreamWrapper>(new IStreamWrapper(*ss.data()));

        Document doc;
        ParseResult ok = doc.ParseStream(*isw.data());
        if (!ok)
            throw runtime_error("eror parsing json");
        if (!doc.IsObject() or !doc.HasMember("biot"))
            throw runtime_error("eror parsing json");
        pBiot.reset(new Biot(env));
        pBiot->SerializeJsonLoad(doc["biot"]);
        pBiot->OnOpen();

    } catch (exception &err) {

        std::cout << err.what() << std::endl;
        if(returnOnFailure)
            returnRejectedBiotToPeer(rpcType, side, d, err.what());
        return;
    }

    if(env.side[side]->ReceiveBiotFromNetwork(pBiot.get(), allowQueueOverflow))
        pBiot.release();
    else
        std::cout << "dropping received biot because side queue is full" << std::endl;
}

void SidesManager::receiveCachedReturnedBiot(int side, const QByteArray &jsonPayload)
{
    cout << "returned biot arriving " << side << endl;

    std::unique_ptr<Biot> pBiot;
    try {
        stringstream ss(std::string(jsonPayload.constData(), jsonPayload.size()));
        IStreamWrapper isw(ss);

        Document doc;
        ParseResult ok = doc.ParseStream(isw);
        if (!ok)
            throw runtime_error("eror parsing json");
        if (!doc.IsObject() or !doc.HasMember("biot"))
            throw runtime_error("eror parsing json");

        pBiot.reset(new Biot(env));
        pBiot->SerializeJsonLoad(doc["biot"]);
        pBiot->OnOpen();
    }
    catch (exception &err) {
        std::cout << "failed to load cached returned biot: " << err.what() << std::endl;
        return;
    }

    if(env.side[side]->ReceiveBiotFromNetwork(pBiot.get(), true))
        pBiot.release();
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
    if(env.settings.m_enableNetworking and !networking.isListening())
    {
        networking.listen(QHostAddress::Any, env.settings.m_networkPort);
        if(networking.isListening())
            std::cout << "listening on port " << networking.serverPort() << std::endl;
        else
        {
            std::cout << "failed to listen on port " << env.settings.m_networkPort << std::endl;

            networking.listen(QHostAddress::Any);
            if(networking.isListening())
                std::cout << "listening on fall back port " << networking.serverPort() << std::endl;
            else
                std::cout << "failed to listen on fall back  port" << std::endl;
        }
    }
    if(!env.settings.m_enableNetworking and networking.isListening())
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
    if(elapse > RECONNECT_INTERVAL_MS)
    {
        previousActionTime = now;

        //std::cout << "tick" << std::endl;
        sideManager.connectConfiguredSides();
    }

}
