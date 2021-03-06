#include "settingsui.h"
#include "settingsbiot.h"
#include "ui_settingsui.h"

SettingsUi::SettingsUi(class CSettings &settingsIn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsUi),
    settings(settingsIn)
{
    ui->setupUi(this);

    ui->biotTab->Init(settingsIn);
    ui->habitatTab->Init(settingsIn);
    ui->networkTab->Init(settingsIn);
}

SettingsUi::~SettingsUi()
{
    delete ui;
}

void SettingsUi::accept()
{
    this->ui->biotTab->Accept(settings);
    this->ui->habitatTab->Accept(settings);
    this->ui->networkTab->Accept(settings);
    settings.SanityCheck();
    settings.Save();
    QDialog::accept();
}

void SettingsUi::reject()
{
    QDialog::reject();
}
