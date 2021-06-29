#ifndef NETWORKING_H
#define NETWORKING_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

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

    void pageComplete(QTcpSocket *client, const char *data, uint32_t size);

};

class SidesManager : public QObject
{
    Q_OBJECT
public:
    SidesManager();
    virtual ~SidesManager();

    void connectToHost(int side, const QString &hostName, quint16 port);
    void getSideStatus(int side, QString &hostPortOut, QString &statusOut);

public slots:
    void netConnected(QTcpSocket *client);
    void netDisconnected(QTcpSocket *client);
    void netReceivedPage(QTcpSocket *client, const char *data, uint32_t size);

signals:
    void sideConnected(int side);
    void sideAssigned(int side);
    void sideDisconnected(int side);

private:
    class Networking networking;
    QTcpSocket *sockets[4];
    QString status[4];

};

#endif // NETWORKING_H
