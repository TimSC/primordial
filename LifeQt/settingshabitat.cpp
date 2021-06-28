#include "settingshabitat.h"
#include "ui_settingshabitat.h"

SettingsHabitat::SettingsHabitat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsHabitat)
{
    ui->setupUi(this);
}

SettingsHabitat::~SettingsHabitat()
{
    delete ui;
}
