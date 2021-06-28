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

void SettingsHabitat::Init(class CSettings &settingsIn)
{
    this->ui->soundCheckBox->setChecked(settingsIn.bSoundOn);
    this->ui->mouseCheckBox->setChecked(settingsIn.bMouse);
    QString popString = QString::asprintf("%d", settingsIn.m_initialPopulation);
    this->ui->populationLineEdit->setText(popString);
    this->ui->frictionComboBox->setCurrentText(SettingFindClosestTextByValue(frictionList, FRICTION_OPTIONS, settingsIn.friction));
    this->ui->solarComboBox->setCurrentText(SettingFindClosestTextByValue(benefitList, BENEFIT_OPTIONS, settingsIn.m_leafEnergy));
}

void SettingsHabitat::Accept(class CSettings &settingsIn)
{
    settingsIn.bSoundOn = this->ui->soundCheckBox->isChecked();
    settingsIn.bMouse = this->ui->mouseCheckBox->isChecked();
    settingsIn.m_initialPopulation = this->ui->populationLineEdit->text().toInt();
    settingsIn.friction = SettingFindValueByText(frictionList, FRICTION_OPTIONS, this->ui->frictionComboBox->currentText());
    settingsIn.m_leafEnergy = SettingFindValueByText(benefitList, BENEFIT_OPTIONS, this->ui->solarComboBox->currentText());
}
