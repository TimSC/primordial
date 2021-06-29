#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <fstream>
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include <QOpenGLWidget>
#include <QScreen>
#include <QLabel>
#include "core/Biots.h"
#include "settingsui.h"
#include "aboutui.h"
#include "rapidjson/writer.h"
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
using namespace std;
using namespace rapidjson;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/icon.ico"));

    QSize size = qApp->screens()[0]->size();

    QRect rect(0, 0, size.width(), size.height());
    QPen whitePen(QColor(255,255,255));
    QGraphicsRectItem *gri = new QGraphicsRectItem(rect);

    QSurfaceFormat fmt;
    fmt.setSwapInterval(0);
    this->ui->openGLWidget->setFormat(fmt);

    gri->setPen(whitePen);

    this->env.options.Reset(rect.x(), rect.y());

    qint64 seed = QDateTime::currentMSecsSinceEpoch();
    //qint64 seed = 100;
    int numBiots = 40;
    this->env.OnNew(*this->ui->openGLWidget, rect, numBiots, seed,
                0, 1, 10);

    this->ui->openGLWidget->SetEnvironment(&this->env);

    lastSimUpdate = 0;
    lastGraphicsUpdate = 0;
    lastStatsUpdate = 0;
    currentTool = "examine";

    this->ui->statusbar->addWidget(new QLabel("Day", nullptr));
    statusDay = new QLineEdit();
    statusDay->setReadOnly(true);
    statusDay->setFixedWidth(50);
    this->ui->statusbar->addWidget(statusDay);
    this->ui->statusbar->addWidget(new QLabel("Population", nullptr));
    statusPopulation = new QLineEdit();
    statusPopulation->setReadOnly(true);
    statusPopulation->setFixedWidth(80);
    this->ui->statusbar->addWidget(statusPopulation);
    this->ui->statusbar->addWidget(new QLabel("Extinctions", nullptr));
    statusExtinctions = new QLineEdit();
    statusExtinctions->setReadOnly(true);
    statusExtinctions->setFixedWidth(50);
    this->ui->statusbar->addWidget(statusExtinctions);

    server.listen(QHostAddress::Any);
    std::cout << "listening on port " << server.serverPort() << std::endl;

    startTimer(10);     // 5-millisecond timer
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    int64_t now = QDateTime::currentMSecsSinceEpoch();


    if(this->ui->actionStart_Simulation->isChecked())
    {
        uint64_t elapsed = now - lastSimUpdate;
        if(elapsed > 20)
        {
            lastSimUpdate = now;
            this->env.Skip();
        }
    }

    int64_t elapsed = now - lastGraphicsUpdate;
    if(elapsed > 40)
    {
        lastGraphicsUpdate = now;
        this->ui->openGLWidget->update();
    }

    elapsed = now - lastStatsUpdate;
    if(env.m_statsList.size() > 0 and elapsed > 500)
    {
        lastStatsUpdate = now;

        const CEnvStats &stats = env.m_statsList.last();
        statusDay->setText(stats.GetDaysStr().c_str());
        statusPopulation->setText(stats.GetPopulationStr().c_str());
        statusExtinctions->setText(stats.GetExtinctionsStr().c_str());
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Address Book"), this->currentFilename.c_str(),
        tr("Primordial Life Files (*.plfj);;All Files (*)"));

    if (fileName.isEmpty())
            return;

    Document d;

    ifstream ifs(fileName.toStdString().c_str());
    IStreamWrapper isw(ifs);

    d.ParseStream(isw);
    this->env.SerializeJsonLoad(d["environment"]);
    this->env.OnOpen();

    this->currentFilename = fileName.toStdString();

}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save As"), this->currentFilename.c_str(),
        tr("Primordial Life Files (*.plfj);;All Files (*)"));

    if (fileName.isEmpty())
            return;

    this->currentFilename = fileName.toStdString();
    Document d;
    d.SetObject();
    Value envJson(kObjectType);
    this->env.SerializeJson(d, envJson);
    d.AddMember("environment", envJson, d.GetAllocator());
    ofstream myfile(fileName.toStdString().c_str());
    OStreamWrapper osw(myfile);
    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);
}

void MainWindow::on_actionNew_triggered()
{
    QRect rect(0, 0, 2000, 1500);
    qint64 seed = QDateTime::currentMSecsSinceEpoch();
    int numBiots = 20;

    this->env.DeleteContents();
    this->env.OnNew(*this->ui->openGLWidget, rect, numBiots, seed,
                                  0, 1, 10);
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionSave_triggered()
{
    if (this->currentFilename.empty())
    {
        on_actionSave_As_triggered();
        return;
    }

    Document d;
    d.SetObject();
    Value envJson(kObjectType);
    this->env.SerializeJson(d, envJson);
    d.AddMember("environment", envJson, d.GetAllocator());
    ofstream myfile(this->currentFilename);
    OStreamWrapper osw(myfile);
    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);
}

void MainWindow::on_actionStart_Simulation_triggered()
{

}

void MainWindow::on_actionLearn_about_Primordial_Life_triggered()
{

}

void MainWindow::on_actionStart_Simulation_triggered(bool checked)
{

}

void MainWindow::on_actionCure_Sicken_triggered(bool checked)
{
    if (checked)
        currentTool = "cure-sicken";
    updateToolMenu();
}

void MainWindow::on_actionExamine_triggered(bool checked)
{
    if (checked)
        currentTool = "examine";
    updateToolMenu();
}

void MainWindow::on_actionFeed_triggered(bool checked)
{
    if (checked)
        currentTool = "feed";
    updateToolMenu();
}

void MainWindow::on_actionMutate_triggered(bool checked)
{
    if (checked)
        currentTool = "mutate";
    updateToolMenu();
}

void MainWindow::on_actionOpenSingle_triggered(bool checked)
{
    if (checked)
        currentTool = "open";
    updateToolMenu();
}

void MainWindow::on_actionRelocate_triggered(bool checked)
{
    if (checked)
        currentTool = "relocate";
    updateToolMenu();
}

void MainWindow::on_actionSaveSingle_triggered(bool checked)
{
    if (checked)
        currentTool = "save";
    updateToolMenu();
}

void MainWindow::on_actionTerminate_triggered(bool checked)
{
    if (checked)
        currentTool = "terminate";
    updateToolMenu();
}

void MainWindow::updateToolMenu()
{
    this->ui->actionCure_Sicken->setChecked(currentTool == "cure-sicken");
    this->ui->actionExamine->setChecked(currentTool == "examine");
    this->ui->actionFeed->setChecked(currentTool == "feed");
    this->ui->actionMutate->setChecked(currentTool == "mutate");
    this->ui->actionOpenSingle->setChecked(currentTool == "open");
    this->ui->actionRelocate->setChecked(currentTool == "relocate");
    this->ui->actionSaveSingle->setChecked(currentTool == "save");
    this->ui->actionTerminate->setChecked(currentTool == "terminate");

    this->ui->openGLWidget->setCurrentTool(currentTool);
}

void MainWindow::on_actionSettings_triggered()
{
    class SettingsUi settingsUi(env.options);
    settingsUi.exec();
    int ret = settingsUi.result();
    if(ret == QDialog::Accepted)
    {

    }
}

void MainWindow::on_actionStatus_Bar_triggered(bool checked)
{
    this->ui->statusbar->setVisible(checked);
}

void MainWindow::on_actionAbout_triggered()
{
    AboutUi aboutUi;
    aboutUi.exec();
}
