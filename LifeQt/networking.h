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

    QTcpSocket *connectToHost(const QString &hostName, quint16 port);
    void sendPage(QTcpSocket *client, const char *data, uint32_t size);

public slots:
    void acceptConnection();
    void clientDisconnected();
    void clientBytesAvailable();
    void connected();

signals:
    void netConnected(QTcpSocket *client);
    void netDisconnected(QTcpSocket *client);
    void netReceivedPage(QTcpSocket *client, const char *data, uint32_t size);

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

    virtual void BiotLeavingSide(int side, Biot *pBiot);
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

public slots:
    void netConnected(QTcpSocket *client);
    void netDisconnected(QTcpSocket *client);
    void netReceivedPage(QTcpSocket *client, const char *data, uint32_t size);

signals:
    void sideConnecting(int side);
    void sideConnected(int side);
    void sideAssigned(int side);
    void sideDisconnected(int side);

private:
    class Environment &env;
    class Networking networking;
    QTcpSocket *sockets[4];
    QString status[4];
    SidesManagerEventRx eventRx;
};

#endif // NETWORKING_H
