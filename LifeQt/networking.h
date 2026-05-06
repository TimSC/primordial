#ifndef NETWORKING_H
#define NETWORKING_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include "core/Environ.h"

const int RX_BUFFER_SIZE = 50*1024;

class Networking : public QTcpServer
{
    Q_OBJECT
public:
    Networking();
    virtual ~Networking();

    void connectToHost(QTcpSocket *socket, const QString &hostName, quint16 port);
    void sendPage(QTcpSocket *client, const char *data, uint32_t size);

public slots:
    void acceptConnection();
    void clientBytesAvailable();
    void clientStateChanged(QAbstractSocket::SocketState socketState);

signals:
    void netStateChanged(QTcpSocket *client, QAbstractSocket::SocketState socketState);
    void netReceivedPage(QTcpSocket *client, const char *data, uint32_t size);
    void netAcceptConnection(QTcpSocket *);

private:
    QList<QTcpSocket *> clients;
    char rxBuffer[RX_BUFFER_SIZE];
    QMap<QTcpSocket *, QByteArray> assembleBuffers;
    std::string magicCode;
    int magicCodeLen;

    void pageComplete(QTcpSocket *client, const char *data, uint32_t size);

};

class SidesManagerEventRx : public SideListener
{
public:
    SidesManagerEventRx(class SidesManager *managerIn);
    virtual ~SidesManagerEventRx();

    virtual bool BiotLeavingSide(int side, Biot *pBiot) override;
    virtual void ReadyToReceive(int sideId, bool ready) override;

    class SidesManager *manager;
};

// ************************************************

struct RecentlySentBiot {
    qint64 sentTime;
    QByteArray jsonPayload;
};

class SidesManager : public QObject
{
    Q_OBJECT
public:
    SidesManager(class Environment &envIn);
    virtual ~SidesManager();

    void connectToHost(int side, const QString &hostName, quint16 port);
    void disconnectSide(int side);
    void getSideStatus(int side, QString &hostPortOut, QString &statusOut, bool &enableConnect);
    quint16 defaultNetworkPort() const;
    void connectConfiguredSides();
    bool autoReconnectEnabled() const;
    void setAutoReconnectEnabled(bool enabled);
    bool biotLeavingSide(int side, Biot *pBiot);
    bool isListening(uint16_t &portOut);
    void readyToReceive(int sideId, bool ready);
    void updateListenMode();

public slots:
    void netAcceptConnection(QTcpSocket *client);
    void netStateChanged(QTcpSocket *client, QAbstractSocket::SocketState socketState);
    void netReceivedPage(QTcpSocket *client, const char *data, uint32_t size);

signals:
    void sideStateChanged(int side, QAbstractSocket::SocketState socketState);
    void sideAssigned(int side);

private:
    void receiveBiotFromNetwork(const QString &rpcType, int side, const QByteArray &d, bool returnOnFailure, bool allowQueueOverflow = false);
    void receiveCachedReturnedBiot(int side, const QByteArray &jsonPayload);
    void returnRejectedBiotToPeer(const QString &rpcType, int side, const QByteArray &d, const char *reason);

    class Environment &env;
    class Networking networking;
    QTcpSocket *sockets[4];
    bool isAssigned[4];
    QString configuredHostPort[4];
    QMap<uint32_t, RecentlySentBiot> recentlySentBiots[4];
    QString status[4];
    SidesManagerEventRx eventRx;
};

// ************************************************

class AutoConnect : public QObject
{
    Q_OBJECT
public:
    AutoConnect(class Environment &envIn, class SidesManager &sideManagerIn);
    virtual ~AutoConnect();

    void TimedUpdate();
private:
    class Environment &env;
    class SidesManager &sideManager;

    int64_t previousActionTime;
};

#endif // NETWORKING_H
