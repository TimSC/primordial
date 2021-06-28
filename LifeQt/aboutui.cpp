#include "aboutui.h"
#include "ui_aboutui.h"

AboutUi::AboutUi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutUi)
{
    ui->setupUi(this);
}

AboutUi::~AboutUi()
{
    delete ui;
}
