#ifndef NETWORKING_H
#define NETWORKING_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
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

    virtual void BiotLeavingSide(int side, Biot *pBiot) override;
    virtual void ReadyToReceive(int sideId, bool ready) override;

    class SidesManager *manager;
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
    void biotLeavingSide(int side, Biot *pBiot);
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

    void receiveBiotFromNetwork(const QString &rpcType, int side, QByteArray &d);

    class Environment &env;
    class Networking networking;
    QTcpSocket *sockets[4];
    bool isAssigned[4];
    QString status[4];
    SidesManagerEventRx eventRx;
};

#endif // NETWORKING_H
