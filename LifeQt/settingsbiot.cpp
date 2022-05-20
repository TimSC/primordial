#include "settingsbiot.h"
#include "ui_settingsbiot.h"
#include "core/Settings.h"

// ***************************************************

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

void SettingsBiot::Init(class CSettings &settingsIn)
{
    this->ui->mutationComboBox->setCurrentText(SettingFindClosestTextByInt(mutationList, MUTATION_OPTIONS, settingsIn.chance));
    this->ui->regenCostcomboBox->setCurrentText(SettingFindClosestTextByInt(regenCostList, REGENCOST_OPTIONS, settingsIn.regenCost));
    this->ui->regenRateComboBox->setCurrentText(SettingFindClosestTextByInt(regenTimeList, REGENTIME_OPTIONS, settingsIn.regenTime));
    this->ui->reproductionComboBox->setCurrentText(SettingFindClosestTextByInt(settingsSexualList, SETTINGS_SEXUAL_OPTIONS, settingsIn.nSexual));

    this->ui->parentAttackCheckBox->setChecked(settingsIn.bParentAttack);
    this->ui->siblingAttackCheckBox->setChecked(settingsIn.bSiblingsAttack);
}

void SettingsBiot::Accept(class CSettings &settingsIn)
{
    settingsIn.chance = SettingFindValueByText(mutationList, MUTATION_OPTIONS, this->ui->mutationComboBox->currentText());
    settingsIn.regenCost = SettingFindValueByText(regenCostList, REGENCOST_OPTIONS, this->ui->regenCostcomboBox->currentText());
    settingsIn.regenTime = SettingFindValueByText(regenTimeList, REGENTIME_OPTIONS, this->ui->regenRateComboBox->currentText());
    settingsIn.nSexual = SettingFindValueByText(settingsSexualList, SETTINGS_SEXUAL_OPTIONS, this->ui->reproductionComboBox->currentText());

    settingsIn.bParentAttack = this->ui->parentAttackCheckBox->isChecked();
    settingsIn.bSiblingsAttack = this->ui->siblingAttackCheckBox->isChecked();
}
