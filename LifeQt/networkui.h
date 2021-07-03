#ifndef NETWORKUI_H
#define NETWORKUI_H

#include "networking.h"
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class NetworkUi;
}

class NetworkUi : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkUi(SidesManager &sidesManagerIn, QWidget *parent = nullptr);
    ~NetworkUi();

private slots:
    void on_connectButton_clicked();

    void on_connectButton_2_clicked();

    void on_connectButton_3_clicked();

    void on_connectButton_4_clicked();

public slots:
    void sideAssigned(int side);
    void sideStateChanged(int side, QAbstractSocket::SocketState);

private:
    Ui::NetworkUi *ui;
    SidesManager &sidesManager;
    void UpdateRow(int side, QLineEdit *lineEdit, QLineEdit *statusEdit, QPushButton *button);
    void ConnectRow(int side, QPushButton *button, QLineEdit *lineEdit);
};

#endif // NETWORKUI_H
