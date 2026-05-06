#include <QCoreApplication>
#include <QByteArray>
#include <QDateTime>
#include <QHostAddress>
#include <QList>
#include <QMap>
#include <QSet>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QtEndian>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

using namespace rapidjson;

namespace {

const int DEFAULT_PORT = 54275;
const int DEFAULT_MAX_CLIENTS = 4096;
const int DEFAULT_MAX_CLIENTS_PER_IP = 16;
const int RX_BUFFER_SIZE = 50 * 1024;
const uint32_t MAX_PAGE_SIZE = 1024 * 1024;
const uint32_t MAX_BIOT_JSON_SIZE = 512 * 1024;
const qint64 LINK_BIOT_INTERVAL_MS = 1000;
const int MAGIC_CODE_LEN = 8;
const char *MAGIC_CODE = "primlife";
const char *RPC_PEER_HELLO = "peerhello1";
const char *RPC_MUX_OPEN = "muxopen001";
const char *RPC_MUX_CLOSE = "muxclose01";
const char *RPC_MUX_FRAME = "muxframe01";
const char *RPC_MUX_ASSIGN = "muxassign1";
const char *RPC_MUX_NO_FREE = "muxnofree1";
const qint64 HELLO_TIMEOUT_MS = 30000;
const qint64 INCOMPLETE_FRAME_TIMEOUT_MS = 10000;
const qint64 RETURN_BIOT_WINDOW_MS = 10000;
const int MAX_INSTANCE_ID_LEN = 128;

struct Client;

struct ClientChannel {
    Client *client = nullptr;
    uint8_t id = 0;
    bool readyToReceive = true;
    ClientChannel *link = nullptr;
    qint64 lastSentBiotTime = 0;
    QMap<uint32_t, qint64> recentlyReceivedBiotIds;
};

struct Client {
    QTcpSocket *socket = nullptr;
    QByteArray assemblyBuffer;
    QString instanceId;
    QString peerAddress;
    qint64 connectTime = 0;
    qint64 lastActivityTime = 0;
    QMap<int, ClientChannel *> channels;
};

uint32_t GetQtCompressedSize(const QByteArray &data)
{
    if(data.size() < (int)sizeof(uint32_t))
        throw std::runtime_error("compressed biot payload is truncated");

    return qFromBigEndian<uint32_t>((const uchar *)data.constData());
}

QByteArray UncompressBiotPayload(const QByteArray &data)
{
    uint32_t uncompressedSize = GetQtCompressedSize(data);
    if(uncompressedSize > MAX_BIOT_JSON_SIZE)
        throw std::runtime_error("compressed biot payload is too large");

    QByteArray uncompressed = qUncompress(data);
    if(uncompressed.isEmpty() || uncompressed.size() > (int)MAX_BIOT_JSON_SIZE)
        throw std::runtime_error("compressed biot payload is invalid");

    return uncompressed;
}

QByteArray DecodeBiotPayload(const QString &rpcType, const QByteArray &frame)
{
    QByteArray payload(frame.mid(10));
    if(rpcType == "transfbiot" || rpcType == "returnbiot")
    {
        if(payload.size() > (int)MAX_BIOT_JSON_SIZE)
            throw std::runtime_error("biot payload is too large");
        return payload;
    }
    if(rpcType == "transbiotc" || rpcType == "returnbioc")
        return UncompressBiotPayload(payload);

    throw std::runtime_error("unknown biot payload type");
}

uint32_t ValidateBiotPayloadAndGetId(const QString &rpcType, const QByteArray &frame)
{
    QByteArray payload = DecodeBiotPayload(rpcType, frame);
    std::stringstream ss(std::string(payload.constData(), payload.size()));
    IStreamWrapper isw(ss);

    Document doc;
    ParseResult ok = doc.ParseStream(isw);
    if(!ok || !doc.IsObject() || !doc.HasMember("biot") || !doc["biot"].IsObject())
        throw std::runtime_error("error parsing biot json");

    const Value &biot = doc["biot"];
    if(!biot.HasMember("m_Id") || !biot["m_Id"].IsUint())
        throw std::runtime_error("biot payload missing id");

    return biot["m_Id"].GetUint();
}

QString ParsePeerInstanceId(const QByteArray &frame)
{
    QByteArray payload = frame.mid(10);
    std::stringstream ss(std::string(payload.constData(), payload.size()));
    IStreamWrapper isw(ss);

    Document doc;
    ParseResult ok = doc.ParseStream(isw);
    if(!ok || !doc.IsObject() || !doc.HasMember("instanceId") || !doc["instanceId"].IsString())
        throw std::runtime_error("error parsing peer hello");

    return QString::fromUtf8(doc["instanceId"].GetString(), doc["instanceId"].GetStringLength());
}

bool IsBiotTransferRpc(const QString &rpcType)
{
    return rpcType == "transfbiot" || rpcType == "transbiotc";
}

QByteArray ReturnBiotFrame(const QString &transferRpcType, uint32_t biotId)
{
    QByteArray frame;
    if(transferRpcType == "transfbiot")
        frame.append("returnbiot");
    else if(transferRpcType == "transbiotc")
        frame.append("returnbioc");
    else
        return frame;

    uint32_t networkBiotId = qToBigEndian<uint32_t>(biotId);
    frame.append((const char *)&networkBiotId, sizeof(networkBiotId));
    return frame;
}

bool ParseReturnBiotId(const QByteArray &frame, uint32_t &biotId)
{
    if(frame.size() < 10 + (int)sizeof(uint32_t))
        return false;
    biotId = qFromBigEndian<uint32_t>((const uchar *)frame.constData() + 10);
    return true;
}

// Strip non-printable ASCII and cap length to prevent log injection.
QString SanitizeInstanceId(const QString &s)
{
    QString result;
    result.reserve(qMin(s.size(), MAX_INSTANCE_ID_LEN));
    for(QChar c : s)
    {
        if(c.unicode() >= 0x20 && c.unicode() < 0x7f)
            result.append(c);
        else
            result.append('?');
        if(result.size() >= MAX_INSTANCE_ID_LEN)
            break;
    }
    return result;
}

} // namespace

class HubServer : public QObject
{
public:
    explicit HubServer(quint16 port, int maxClientsIn, int maxClientsPerIpIn, QObject *parent = nullptr) :
        QObject(parent),
        maxClients(maxClientsIn),
        maxClientsPerIp(maxClientsPerIpIn)
    {
        connect(&server, &QTcpServer::newConnection, this, [this]() { acceptConnections(); });
        connect(&linkTimer, &QTimer::timeout, this, [this]() { reviewLinks(); sweepTimeouts(); });
        linkTimer.start(1000);
        connect(&minuteTimer, &QTimer::timeout, this, [this]() { logMinuteStats(); });
        minuteTimer.start(60000);

        if(!server.listen(QHostAddress::Any, port))
            throw std::runtime_error(server.errorString().toStdString());

        std::cout << "hub listening on port " << server.serverPort()
                  << ", max clients " << maxClients
                  << ", max per ip " << maxClientsPerIp << std::endl;
    }

private:
    QTcpServer server;
    QMap<QTcpSocket *, Client *> clients;
    QMap<QString, int> connectionsPerIp;
    QTimer linkTimer;
    QTimer minuteTimer;
    int maxClients;
    int maxClientsPerIp;
    int biotsRelayedThisMinute = 0;
    int biotsRateLimitedThisMinute = 0;

    void acceptConnections()
    {
        while(QTcpSocket *socket = server.nextPendingConnection())
        {
            if(clients.size() >= maxClients)
            {
                sendFrame(socket, QByteArray("nofreeside{}"));
                socket->disconnectFromHost();
                continue;
            }

            QString peerAddr = socket->peerAddress().toString();
            if(connectionsPerIp.value(peerAddr, 0) >= maxClientsPerIp)
            {
                socket->disconnectFromHost();
                continue;
            }

            qint64 now = QDateTime::currentMSecsSinceEpoch();
            Client *client = new Client;
            client->socket = socket;
            client->peerAddress = peerAddr;
            client->connectTime = now;
            client->lastActivityTime = now;
            clients[socket] = client;
            connectionsPerIp[peerAddr]++;

            connect(socket, &QTcpSocket::readyRead, this, [this, socket]() { readClient(socket); });
            connect(socket, &QTcpSocket::disconnected, this, [this, socket]() { removeClient(socket); });

            std::cout << "client connected, total=" << clients.size() << std::endl;
        }
    }

    void readClient(QTcpSocket *socket)
    {
        Client *client = findClient(socket);
        if(client == nullptr)
            return;

        char rxBuffer[RX_BUFFER_SIZE];
        while(socket->bytesAvailable())
        {
            qint64 readBytes = socket->read(rxBuffer, sizeof(rxBuffer));
            if(readBytes <= 0)
                return;

            client->assemblyBuffer.append(rxBuffer, readBytes);
            client->lastActivityTime = QDateTime::currentMSecsSinceEpoch();
            processAssemblyBuffer(*client);
        }
    }

    void processAssemblyBuffer(Client &client)
    {
        while(client.assemblyBuffer.size() >= MAGIC_CODE_LEN + (int)sizeof(uint32_t))
        {
            if(client.assemblyBuffer.left(MAGIC_CODE_LEN) != MAGIC_CODE)
            {
                std::cout << "disconnecting client with invalid frame magic" << std::endl;
                client.socket->disconnectFromHost();
                return;
            }

            uint32_t payloadSize = qFromBigEndian<uint32_t>((const uchar *)&client.assemblyBuffer.constData()[MAGIC_CODE_LEN]);
            if(payloadSize > MAX_PAGE_SIZE)
            {
                std::cout << "disconnecting client with oversized frame" << std::endl;
                client.socket->disconnectFromHost();
                return;
            }

            int frameSize = MAGIC_CODE_LEN + sizeof(uint32_t) + payloadSize;
            if(client.assemblyBuffer.size() < frameSize)
                return;

            QByteArray frame = client.assemblyBuffer.mid(MAGIC_CODE_LEN + sizeof(uint32_t), payloadSize);
            client.assemblyBuffer.remove(0, frameSize);
            handleFrame(client, frame);
        }
    }

    void handleFrame(Client &client, const QByteArray &frame)
    {
        QString rpcType = frame.left(10);
        if(rpcType == RPC_PEER_HELLO)
        {
            receivePeerHello(client, frame);
            return;
        }

        if(rpcType == RPC_MUX_OPEN)
        {
            receiveMuxOpen(client, frame);
            return;
        }

        if(rpcType == RPC_MUX_CLOSE)
        {
            receiveMuxClose(client, frame);
            return;
        }

        if(rpcType != RPC_MUX_FRAME || frame.size() < 11)
        {
            std::cout << "dropping non-multiplexed frame" << std::endl;
            return;
        }

        int channelId = (uint8_t)frame[10];
        ClientChannel *channel = client.channels.value(channelId, nullptr);
        if(channel == nullptr)
        {
            std::cout << "dropping frame for unopened channel " << channelId << std::endl;
            return;
        }

        QByteArray channelFrame = frame.mid(11);
        rpcType = channelFrame.left(10);

        if(rpcType == "sidereadyy")
        {
            channel->readyToReceive = true;
            return;
        }

        if(rpcType == "sidunready")
        {
            channel->readyToReceive = false;
            return;
        }

        if(IsBiotTransferRpc(rpcType))
        {
            relayBiot(*channel, rpcType, channelFrame);
            return;
        }

        if(rpcType == "returnbiot" || rpcType == "returnbioc" || rpcType == "biotaccept")
        {
            relayReturnBiot(*channel, rpcType, channelFrame);
            return;
        }
    }

    void receiveMuxOpen(Client &client, const QByteArray &frame)
    {
        QByteArray payload = frame.mid(10);
        if(payload.size() != 1)
            return;

        int channelId = (uint8_t)payload[0];
        if(client.channels.contains(channelId))
            return;

        if(totalChannels() >= maxClients)
        {
            QByteArray noFree(RPC_MUX_NO_FREE);
            noFree.append((char)channelId);
            sendFrame(client.socket, noFree);
            return;
        }

        ClientChannel *channel = new ClientChannel;
        channel->client = &client;
        channel->id = (uint8_t)channelId;
        client.channels[channelId] = channel;

        QByteArray assigned(RPC_MUX_ASSIGN);
        assigned.append((char)channelId);
        sendFrame(client.socket, assigned);
    }

    void receiveMuxClose(Client &client, const QByteArray &frame)
    {
        QByteArray payload = frame.mid(10);
        if(payload.size() != 1)
            return;

        int channelId = (uint8_t)payload[0];
        ClientChannel *channel = client.channels.take(channelId);
        if(channel == nullptr)
            return;

        unlink(*channel);
        delete channel;

        if(client.channels.isEmpty())
            client.socket->disconnectFromHost();
    }

    void receivePeerHello(Client &client, const QByteArray &frame)
    {
        try {
            client.instanceId = SanitizeInstanceId(ParsePeerInstanceId(frame));
        }
        catch(const std::exception &err) {
            std::cout << "invalid peer hello: " << err.what() << std::endl;
            return;
        }

        for(auto it = client.channels.begin(); it != client.channels.end(); ++it)
        {
            ClientChannel *channel = it.value();
            if(channel->link != nullptr && isLoopbackPair(client, *channel->link->client))
                unlink(*channel);
        }
    }

    void relayBiot(ClientChannel &channel, const QString &rpcType, const QByteArray &frame)
    {
        uint32_t biotId = 0;
        try {
            biotId = ValidateBiotPayloadAndGetId(rpcType, frame);
        }
        catch(const std::exception &err) {
            std::cout << "dropping invalid biot: " << err.what() << std::endl;
            return;
        }

        if(channel.link == nullptr)
        {
            sendReturnBiot(channel, rpcType, biotId);
            return;
        }

        if(!channel.link->readyToReceive)
        {
            std::cout << "returning biot " << biotId << ": linked client is not ready" << std::endl;
            sendReturnBiot(channel, rpcType, biotId);
            return;
        }

        qint64 now = QDateTime::currentMSecsSinceEpoch();
        if(now - channel.lastSentBiotTime < LINK_BIOT_INTERVAL_MS)
        {
            biotsRateLimitedThisMinute++;
            sendReturnBiot(channel, rpcType, biotId);
            return;
        }

        channel.lastSentBiotTime = now;
        channel.link->recentlyReceivedBiotIds[biotId] = now;
        sendChannelFrame(*channel.link, frame);
    }

    void relayReturnBiot(ClientChannel &channel, const QString &rpcType, const QByteArray &frame)
    {
        uint32_t biotId = 0;
        if(!ParseReturnBiotId(frame, biotId))
        {
            std::cout << "dropping return biot: malformed frame" << std::endl;
            return;
        }

        auto it = channel.recentlyReceivedBiotIds.find(biotId);
        if(it == channel.recentlyReceivedBiotIds.end())
        {
            std::cout << "dropping " << rpcType.toStdString() << " " << biotId << ": no matching relayed biot" << std::endl;
            return;
        }

        qint64 age = QDateTime::currentMSecsSinceEpoch() - it.value();
        channel.recentlyReceivedBiotIds.erase(it);

        if(age > RETURN_BIOT_WINDOW_MS)
        {
            std::cout << "dropping " << rpcType.toStdString() << " " << biotId << ": window expired" << std::endl;
            return;
        }

        if(channel.link != nullptr)
        {
            if(rpcType == "biotaccept")
                biotsRelayedThisMinute++;
            sendChannelFrame(*channel.link, frame);
        }
    }

    void logMinuteStats()
    {
        int unlinked = 0;
        int linked = 0;
        for(auto it = clients.begin(); it != clients.end(); ++it)
        {
            Client *client = it.value();
            for(auto channelIt = client->channels.begin(); channelIt != client->channels.end(); ++channelIt)
            {
                if(channelIt.value()->link == nullptr)
                    unlinked++;
                else
                    linked++;
            }
        }
        linked /= 2;
        std::cout << "stats: connected=" << clients.size()
                  << " channels=" << totalChannels()
                  << " links=" << linked
                  << " biots_last_min=" << biotsRelayedThisMinute
                  << " rate_limited=" << biotsRateLimitedThisMinute
                  << " unlinked=" << unlinked << std::endl;
        biotsRelayedThisMinute = 0;
        biotsRateLimitedThisMinute = 0;
    }

    void reviewLinks()
    {
        QList<ClientChannel *> unlinked;
        for(auto it = clients.begin(); it != clients.end(); ++it)
        {
            Client *client = it.value();
            if(client->instanceId.isEmpty())
                continue;
            for(auto channelIt = client->channels.begin(); channelIt != client->channels.end(); ++channelIt)
            {
                ClientChannel *channel = channelIt.value();
                if(channel->link == nullptr)
                    unlinked.append(channel);
            }
        }

        while(!unlinked.isEmpty())
        {
            ClientChannel *first = unlinked.takeFirst();
            ClientChannel *second = nullptr;
            for(int i=0; i<unlinked.size(); i++)
            {
                if(!isLoopbackPair(*first->client, *unlinked[i]->client))
                {
                    second = unlinked.takeAt(i);
                    break;
                }
            }

            if(second == nullptr)
                continue;

            first->link = second;
            second->link = first;
            std::cout << "linked channels"
                      << " a=" << first->client->instanceId.toStdString() << ":" << (int)first->id
                      << " b=" << second->client->instanceId.toStdString() << ":" << (int)second->id << std::endl;
        }
    }

    void sweepTimeouts()
    {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        QList<QTcpSocket *> toDisconnect;

        for(auto it = clients.begin(); it != clients.end(); ++it)
        {
            Client *client = it.value();

            if(client->instanceId.isEmpty() && now - client->connectTime > HELLO_TIMEOUT_MS)
            {
                std::cout << "disconnecting client: no hello within timeout" << std::endl;
                toDisconnect.append(it.key());
                continue;
            }

            if(!client->assemblyBuffer.isEmpty() && now - client->lastActivityTime > INCOMPLETE_FRAME_TIMEOUT_MS)
            {
                std::cout << "disconnecting client: incomplete frame timeout" << std::endl;
                toDisconnect.append(it.key());
                continue;
            }

            for(auto channelIt = client->channels.begin(); channelIt != client->channels.end(); ++channelIt)
            {
                ClientChannel *channel = channelIt.value();
                auto bidIt = channel->recentlyReceivedBiotIds.begin();
                while(bidIt != channel->recentlyReceivedBiotIds.end())
                {
                    if(now - bidIt.value() > RETURN_BIOT_WINDOW_MS)
                        bidIt = channel->recentlyReceivedBiotIds.erase(bidIt);
                    else
                        ++bidIt;
                }
            }
        }

        for(QTcpSocket *socket : toDisconnect)
            socket->disconnectFromHost();
    }

    bool isLoopbackPair(const Client &a, const Client &b) const
    {
        if(&a == &b)
            return true;
        return !a.instanceId.isEmpty() && a.instanceId == b.instanceId;
    }

    void unlink(ClientChannel &channel)
    {
        ClientChannel *other = channel.link;
        channel.link = nullptr;
        if(other != nullptr && other->link == &channel)
            other->link = nullptr;
    }

    void unlinkAll(Client &client)
    {
        for(auto it = client.channels.begin(); it != client.channels.end(); ++it)
            unlink(*it.value());
    }

    void removeClient(QTcpSocket *socket)
    {
        Client *client = findClient(socket);
        if(client != nullptr)
        {
            unlinkAll(*client);

            int count = connectionsPerIp.value(client->peerAddress, 0) - 1;
            if(count <= 0)
                connectionsPerIp.remove(client->peerAddress);
            else
                connectionsPerIp[client->peerAddress] = count;

            qDeleteAll(client->channels);
            client->channels.clear();
            delete client;
        }

        clients.remove(socket);
        socket->deleteLater();
        std::cout << "client disconnected, total=" << clients.size() << std::endl;
    }

    Client *findClient(QTcpSocket *socket)
    {
        auto it = clients.find(socket);
        if(it == clients.end())
            return nullptr;
        return it.value();
    }

    int totalChannels() const
    {
        int total = 0;
        for(auto it = clients.begin(); it != clients.end(); ++it)
            total += it.value()->channels.size();
        return total;
    }

    void sendReturnBiot(ClientChannel &channel, const QString &rpcType, uint32_t biotId)
    {
        QByteArray frame = ReturnBiotFrame(rpcType, biotId);
        if(!frame.isEmpty())
            sendChannelFrame(channel, frame);
    }

    void sendChannelFrame(ClientChannel &channel, const QByteArray &frame)
    {
        QByteArray muxFrame(RPC_MUX_FRAME);
        muxFrame.append((char)channel.id);
        muxFrame.append(frame);
        sendFrame(channel.client->socket, muxFrame);
    }

    void sendFrame(QTcpSocket *socket, const QByteArray &frame)
    {
        if(socket == nullptr || frame.size() > (int)MAX_PAGE_SIZE)
            return;

        socket->write(MAGIC_CODE, MAGIC_CODE_LEN);
        uint32_t pageSize = qToBigEndian<uint32_t>((uint32_t)frame.size());
        socket->write((const char *)&pageSize, sizeof(pageSize));
        socket->write(frame.constData(), frame.size());
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    quint16 port = DEFAULT_PORT;
    int maxClients = DEFAULT_MAX_CLIENTS;
    int maxClientsPerIp = DEFAULT_MAX_CLIENTS_PER_IP;
    QStringList args = app.arguments();
    for(int i=1; i<args.size(); i++)
    {
        if(args[i] == "--port" && i + 1 < args.size())
        {
            bool ok = false;
            uint parsedPort = args[++i].toUInt(&ok);
            if(ok && parsedPort <= 65535)
                port = (quint16)parsedPort;
        }
        else if(args[i] == "--max-clients" && i + 1 < args.size())
        {
            bool ok = false;
            int parsedMaxClients = args[++i].toInt(&ok);
            if(ok && parsedMaxClients > 0)
                maxClients = parsedMaxClients;
        }
        else if(args[i] == "--max-clients-per-ip" && i + 1 < args.size())
        {
            bool ok = false;
            int parsed = args[++i].toInt(&ok);
            if(ok && parsed > 0)
                maxClientsPerIp = parsed;
        }
    }

    try {
        HubServer hub(port, maxClients, maxClientsPerIp);
        return app.exec();
    }
    catch(const std::exception &err) {
        std::cerr << "failed to start hub: " << err.what() << std::endl;
        return 1;
    }
}
