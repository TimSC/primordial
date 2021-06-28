#include "settingsbiot.h"
#include "ui_settingsbiot.h"

SettingsBiot::SettingsBiot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsBiot)
{
    ui->setupUi(this);
}

SettingsBiot::~SettingsBiot()
{
    delete ui;
}
