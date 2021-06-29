#ifndef NETWORKUI_H
#define NETWORKUI_H

#include "networking.h"
#include <QDialog>

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
    void sideChangedStatus(int side);

private:
    Ui::NetworkUi *ui;
    SidesManager &sidesManager;
};

#endif // NETWORKUI_H
