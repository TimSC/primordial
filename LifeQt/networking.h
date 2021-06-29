#ifndef NETWORKING_H
#define NETWORKING_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class PrimordialServer : public QTcpServer
{
    Q_OBJECT
public:
    PrimordialServer();
    virtual ~PrimordialServer();

public slots:
    void acceptConnection();
    void clientDisconnected();
    void clientBytesAvailable();

private:
    QList<QTcpSocket *> clients;
    char rxBuffer[50*1024];

};

#endif // NETWORKING_H
