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
const int RX_BUFFER_SIZE = 50 * 1024;
const uint32_t MAX_PAGE_SIZE = 1024 * 1024;
const uint32_t MAX_BIOT_JSON_SIZE = 512 * 1024;
const qint64 LINK_BIOT_INTERVAL_MS = 1000;
const int MAGIC_CODE_LEN = 8;
const char *MAGIC_CODE = "primlife";
const char *RPC_PEER_HELLO = "peerhello1";

struct Client {
    QTcpSocket *socket = nullptr;
    QByteArray assemblyBuffer;
    QString instanceId;
    bool readyToReceive = true;
    Client *link = nullptr;
    qint64 lastSentBiotTime = 0;
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

} // namespace

class HubServer : public QObject
{
public:
    explicit HubServer(quint16 port, int maxClientsIn, QObject *parent = nullptr) :
        QObject(parent),
        maxClients(maxClientsIn)
    {
        connect(&server, &QTcpServer::newConnection, this, [this]() { acceptConnections(); });
        connect(&linkTimer, &QTimer::timeout, this, [this]() { reviewLinks(); });
        linkTimer.start(1000);

        if(!server.listen(QHostAddress::Any, port))
            throw std::runtime_error(server.errorString().toStdString());

        std::cout << "hub listening on port " << server.serverPort()
                  << ", max clients " << maxClients << std::endl;
    }

private:
    QTcpServer server;
    QMap<QTcpSocket *, Client *> clients;
    QTimer linkTimer;
    int maxClients;

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

            Client *client = new Client;
            client->socket = socket;
            clients[socket] = client;

            connect(socket, &QTcpSocket::readyRead, this, [this, socket]() { readClient(socket); });
            connect(socket, &QTcpSocket::disconnected, this, [this, socket]() { removeClient(socket); });

            sendFrame(socket, QByteArray("assignside{}"));
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

        if(rpcType == "sidereadyy")
        {
            client.readyToReceive = true;
            return;
        }

        if(rpcType == "sidunready")
        {
            client.readyToReceive = false;
            return;
        }

        if(IsBiotTransferRpc(rpcType))
        {
            relayBiot(client, rpcType, frame);
            return;
        }

        if(rpcType == "returnbiot" || rpcType == "returnbioc")
        {
            if(client.link != nullptr)
                sendFrame(client.link->socket, frame);
            return;
        }
    }

    void receivePeerHello(Client &client, const QByteArray &frame)
    {
        try {
            client.instanceId = ParsePeerInstanceId(frame);
        }
        catch(const std::exception &err) {
            std::cout << "invalid peer hello: " << err.what() << std::endl;
            return;
        }

        if(client.link != nullptr && isLoopbackPair(client, *client.link))
            unlink(client);
    }

    void relayBiot(Client &client, const QString &rpcType, const QByteArray &frame)
    {
        uint32_t biotId = 0;
        try {
            biotId = ValidateBiotPayloadAndGetId(rpcType, frame);
        }
        catch(const std::exception &err) {
            std::cout << "dropping invalid biot: " << err.what() << std::endl;
            return;
        }

        if(client.link == nullptr)
        {
            std::cout << "returning biot " << biotId << ": client has no hub link" << std::endl;
            sendReturnBiot(client, rpcType, biotId);
            return;
        }

        if(!client.link->readyToReceive)
        {
            std::cout << "returning biot " << biotId << ": linked client is not ready" << std::endl;
            sendReturnBiot(client, rpcType, biotId);
            return;
        }

        qint64 now = QDateTime::currentMSecsSinceEpoch();
        if(now - client.lastSentBiotTime < LINK_BIOT_INTERVAL_MS)
        {
            std::cout << "returning biot " << biotId << ": link rate limit exceeded" << std::endl;
            sendReturnBiot(client, rpcType, biotId);
            return;
        }

        client.lastSentBiotTime = now;
        sendFrame(client.link->socket, frame);
    }

    void reviewLinks()
    {
        QList<Client *> unlinked;
        for(auto it = clients.begin(); it != clients.end(); ++it)
        {
            Client *client = it.value();
            if(client->link == nullptr)
                unlinked.append(client);
        }

        while(!unlinked.isEmpty())
        {
            Client *first = unlinked.takeFirst();
            Client *second = nullptr;
            for(int i=0; i<unlinked.size(); i++)
            {
                if(!isLoopbackPair(*first, *unlinked[i]))
                {
                    second = unlinked.takeAt(i);
                    break;
                }
            }

            if(second == nullptr)
                continue;

            first->link = second;
            second->link = first;
            std::cout << "linked clients"
                      << " a=" << first->instanceId.toStdString()
                      << " b=" << second->instanceId.toStdString() << std::endl;
        }
    }

    bool isLoopbackPair(const Client &a, const Client &b) const
    {
        if(&a == &b)
            return true;
        return !a.instanceId.isEmpty() && a.instanceId == b.instanceId;
    }

    void unlink(Client &client)
    {
        Client *other = client.link;
        client.link = nullptr;
        if(other != nullptr && other->link == &client)
            other->link = nullptr;
    }

    void removeClient(QTcpSocket *socket)
    {
        Client *client = findClient(socket);
        if(client != nullptr)
        {
            unlink(*client);
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

    void sendReturnBiot(Client &client, const QString &rpcType, uint32_t biotId)
    {
        QByteArray frame = ReturnBiotFrame(rpcType, biotId);
        if(!frame.isEmpty())
            sendFrame(client.socket, frame);
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
    }

    try {
        HubServer hub(port, maxClients);
        return app.exec();
    }
    catch(const std::exception &err) {
        std::cerr << "failed to start hub: " << err.what() << std::endl;
        return 1;
    }
}
