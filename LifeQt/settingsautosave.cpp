#include "settingsautosave.h"
#include "ui_settingsautosave.h"

SettingsAutosave::SettingsAutosave(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsAutosave)
{
    ui->setupUi(this);
}

SettingsAutosave::~SettingsAutosave()
{
    delete ui;
}

void SettingsAutosave::Init(class CSettings &settingsIn)
{
    ui->enableAutosaveCheckBox->setChecked(settingsIn.bAutosave);
    ui->autosaveMinutesSpinBox->setValue(settingsIn.autosaveMinutes);
}

void SettingsAutosave::Accept(class CSettings &settingsIn)
{
    settingsIn.bAutosave = ui->enableAutosaveCheckBox->isChecked();
    settingsIn.autosaveMinutes = ui->autosaveMinutesSpinBox->value();
}
