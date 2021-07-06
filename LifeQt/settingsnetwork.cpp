#include "settingsnetwork.h"
#include "ui_settingsnetwork.h"


SettingsNetwork::SettingsNetwork(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsNetwork)
{
    ui->setupUi(this);

    ui->connectListView->setModel(&connectList);
}

SettingsNetwork::~SettingsNetwork()
{
    delete ui;
}

void SettingsNetwork::Init(class CSettings &settingsIn)
{
    this->ui->enableNetworkCheckBox->setChecked(settingsIn.m_enableNetworking);
    this->ui->portLineEdit->setText(QString::asprintf("%d", settingsIn.m_networkPort));

    connectList.setStringList(settingsIn.m_connectList);
}

void SettingsNetwork::Accept(class CSettings &settingsIn)
{
    settingsIn.m_enableNetworking = this->ui->enableNetworkCheckBox->isChecked();
    settingsIn.m_networkPort = this->ui->portLineEdit->text().toUInt();

    settingsIn.m_connectList = connectList.stringList();
}

void SettingsNetwork::on_addPushButton_clicked()
{
    //With help from https://www.bogotobogo.com/Qt/Qt5_QListView_QStringListModel_ModelView_MVC.php
    QString addr = this->ui->addressLineEdit->text();
    int rowNum = connectList.rowCount();
    connectList.insertRows(rowNum, 1);

    QModelIndex index = connectList.index(rowNum);
    connectList.setData(index, QVariant(addr));
}

void SettingsNetwork::on_removePushButton_clicked()
{
    connectList.removeRows(ui->connectListView->currentIndex().row(), 1);
}
