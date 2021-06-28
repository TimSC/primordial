#include "settingsui.h"
#include "ui_settingsui.h"

SettingsUi::SettingsUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsUi)
{
    ui->setupUi(this);
}

SettingsUi::~SettingsUi()
{
    delete ui;
}
