#include "dockviewbiot.h"
#include "core/Biots.h"
#include "ui_dockviewbiot.h"

EnvironmentListenerToDockViewBiot::EnvironmentListenerToDockViewBiot()
{
    dockViewBiot = nullptr;

}

EnvironmentListenerToDockViewBiot::~EnvironmentListenerToDockViewBiot()
{

}

void EnvironmentListenerToDockViewBiot::BiotUpdated(Biot* pBiot)
{
    if(dockViewBiot)
        dockViewBiot->BiotUpdated(pBiot);
}

// ***********************************************

DockViewBiot::DockViewBiot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DockViewBiot)
{
    ui->setupUi(this);
    currentBiotId = 0;
    env = nullptr;
    listenAdapter.dockViewBiot = this;
}

DockViewBiot::~DockViewBiot()
{
    delete ui;
}

void DockViewBiot::SelectedBiot(uint32_t biotId)
{

    currentBiotId = biotId;
}

void DockViewBiot::BiotUpdated(Biot* pBiot)
{
    if(pBiot->m_Id != currentBiotId) return;

    this->ui->nameEdit->setText(pBiot->GetName().c_str());
    this->ui->generationEdit->setText(QString::asprintf("%d", pBiot->m_generation));

    //this->ui->speciesComboBox->setEditText(pBiot->);
    if(pBiot->trait.IsAsexual())
    {
        this->ui->orientationComboBox->setEditText("Asexual");
    }
    else
    {
        if(pBiot->trait.IsMale())
            this->ui->orientationComboBox->setEditText("Male");
        else
            this->ui->orientationComboBox->setEditText("Female");
    }
    this->ui->daysOldEdit->setText(QString::asprintf("%f", pBiot->m_age * 0.05 / CEnvStats::SAMPLE_TIME));
    this->ui->lifespanEdit->setText(QString::asprintf("%f", pBiot->m_maxAge * 0.05 / CEnvStats::SAMPLE_TIME));
    this->ui->energyEdit->setText(QString::asprintf("%d", pBiot->energy));
    this->ui->sickTimeEdit->setText(QString::asprintf("%d", pBiot->m_nSick));

    this->ui->childredEdit->setText(pBiot->m_sWorldName.c_str());
    //this->ui->fertileEdit->setText(pBiot->fert);
    this->ui->childredEdit->setText(QString::asprintf("%d", pBiot->m_totalChildren));
}

void DockViewBiot::on_applyButton_clicked()
{

}
