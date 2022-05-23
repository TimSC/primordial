#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include <QOpenGLWidget>
#include <QScreen>
#include <QLabel>
#include <QByteArray>
#include <QWindow>
#include <QStandardPaths>
#include "core/Biots.h"
#include "settingsui.h"
#include "aboutui.h"
#include "networkui.h"
#include "core/json.h"
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
using namespace std;
using namespace rapidjson;

MainApp::MainApp(): sidesManager(env),
    autoConnect(env, sidesManager)
{
    lastSimUpdate = 0;
    lastFuzz = 0;
}

MainApp::~MainApp()
{


}

void MainApp::TimedUpdate(bool running)
{
    int64_t now = QDateTime::currentMSecsSinceEpoch();

    if(running)
    {
        uint64_t elapsed = now - lastSimUpdate;
        if(elapsed > 20)
        {
            lastSimUpdate = now;
            this->env.Update();
        }

        uint64_t elapsed2 = now - lastFuzz;
        if(elapsed2 > 1000)
        {
            lastFuzz = now;
            this->env.Fuzz();
        }
    }
    else
        lastSimUpdate = now;
}

void MainApp::timerEvent(QTimerEvent *event)
{

}

void MainApp::closeEvent(QCloseEvent *ev)
{
    if(env.settings.bSaveOnQuit)
    {
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir pathObj(path);
        if (!pathObj.exists())
            pathObj.mkpath(".");

        QString fina = pathObj.filePath("current.plfj");

        Save(fina.toStdString());
    }

    ev->accept();
}

void MainApp::Save(const std::string &filename)
{
    QFileInfo fi(filename.c_str());
    QString compSuffix2 = fi.completeSuffix();

    Document d;
    d.SetObject();
    Value envJson(kObjectType);
    this->env.SerializeJson(d, envJson);
    d.AddMember("environment", envJson, d.GetAllocator());

    if(compSuffix2 == "plfc")
    {
        //Write compressed
        stringstream ss;
        OStreamWrapper osw(ss);
        Writer<OStreamWrapper> writer(osw);
        d.Accept(writer);

        string dat = ss.str();
        QByteArray dat2(qCompress(dat.data(), dat.size()));

        ofstream myfile(filename.c_str(), std::ios::binary);

        myfile.write(dat2.data(), dat2.size());
    }
    else
    {
        //Write uncompressed
        ofstream myfile(filename.c_str());
        OStreamWrapper osw(myfile);
        Writer<OStreamWrapper> writer(osw);
        d.Accept(writer);
    }

}

void MainApp::Load(const std::string &fileName)
{
    QFileInfo fi(fileName.c_str());
    QString compSuffix = fi.completeSuffix();
    QSharedPointer<IStreamWrapper> isw;
    QSharedPointer<stringstream> ss;
    QSharedPointer<ifstream> ifs;

    if(compSuffix == "plfc")
    {
        //Load compressed file
        QFile fil(fileName.c_str());
        fil.open(QIODevice::ReadOnly);
        QByteArray dat = fil.readAll();
        QByteArray dat2(qUncompress(dat));

        ss  = QSharedPointer<stringstream>(new stringstream(dat2.constData()));
        isw = QSharedPointer<IStreamWrapper>(new IStreamWrapper(*ss.data()));
    }
    else
    {
        //Load uncompressed file
        ifs = QSharedPointer<ifstream>(new ifstream(fileName.c_str()));
        isw = QSharedPointer<IStreamWrapper>(new IStreamWrapper(*ifs.data()));
    }

    try {
        Document d;
        IStreamWrapper *pisw = isw.data();
        ParseResult ok = d.ParseStream(*pisw);
        if (!ok)
            throw runtime_error("eror parsing json");
        if (!d.IsObject() or !d.HasMember("environment"))
            throw runtime_error("eror parsing json");
        this->env.SerializeJsonLoad(d["environment"]);

    } catch (exception &err) {

        std::cout << err.what() << std::endl;
        return;
    }

    this->env.OnOpen();

}

// *******************************************

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    lastGraphicsUpdate = 0;
    lastStatsUpdate = 0;

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/icon.ico"));

    QSize size = qApp->screens()[0]->size();

    QRect rect(0, 0, size.width(), size.height());
    //QRect rect(0, 0, 640, 480);
    QPen whitePen(QColor(255,255,255));
    QGraphicsRectItem *gri = new QGraphicsRectItem(rect);

    QSurfaceFormat fmt;
    fmt.setSwapInterval(0);
    this->ui->openGLWidget->setFormat(fmt);

    gri->setPen(whitePen);

    this->app.env.settings.Reset(rect.x(), rect.y());

    qint64 seed = QDateTime::currentMSecsSinceEpoch();
    //qint64 seed = 100;
    int numBiots = this->app.env.settings.m_initialPopulation;
    this->app.env.OnNew(*this->ui->openGLWidget, rect, numBiots, seed,
                0, 1, 10);

    this->ui->openGLWidget->SetEnvironment(&this->app.env);

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

    this->ui->statusbar->addWidget(new QLabel("Network", nullptr));
    statusNetwork = new QLineEdit();
    statusNetwork->setReadOnly(true);
    statusNetwork->setFixedWidth(100);
    this->ui->statusbar->addWidget(statusNetwork);

    ui->dockViewBiot->setVisible(false);
    connect(this->ui->openGLWidget, SIGNAL(SelectedBiot(uint32_t)), this, SLOT(SelectedBiot(uint32_t)));
    class DockViewBiot *dvb = qobject_cast<class DockViewBiot *>(this->ui->dockViewBiot->widget());
    connect(this->ui->openGLWidget, SIGNAL(SelectedBiot(uint32_t)), dvb, SLOT(SelectedBiot(uint32_t)));
    app.env.AddListener(&dvb->listenAdapter);
    dvb->env = &app.env;
    connect(this->ui->openGLWidget, SIGNAL(ExitFullscreen()), this, SLOT(ExitFullscreen()));

    if(this->app.env.settings.bSaveOnQuit)
    {
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QString fina = QDir(path).filePath("current.plfj");
        if(QFileInfo::exists(fina))
        {
            this->app.Load(fina.toStdString());
        }
    }

    installEventFilter(this);
    startTimer(10);     // 10-millisecond timer
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    int64_t now = QDateTime::currentMSecsSinceEpoch();

    app.TimedUpdate(this->ui->actionStart_Simulation->isChecked());

    int64_t elapsed = now - lastGraphicsUpdate;
    if(elapsed > 40)
    {
        lastGraphicsUpdate = now;
        this->ui->openGLWidget->update();
    }

    elapsed = now - lastStatsUpdate;
    if(app.env.m_statsList.size() > 0 and elapsed > 500)
    {
        lastStatsUpdate = now;

        const CEnvStats &stats = app.env.m_statsList.last();
        statusDay->setText(stats.GetDaysStr().c_str());
        statusPopulation->setText(QString::asprintf("%d", app.env.GetPopulation()));
        statusExtinctions->setText(stats.GetExtinctionsStr().c_str());
        uint16_t port=0;
        bool isListening = app.sidesManager.isListening(port);
        if(isListening)
        {
            QString newTxt = QString::asprintf("TCP port %d", port);
            if(statusNetwork->text() != newTxt)
                statusNetwork->setText(newTxt);
        }
        else
            statusNetwork->setText("");
    }

    app.autoConnect.TimedUpdate();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Address Book"), this->currentFilename.c_str(),
        tr("Primordial Life Files (*.plfc, *.plfj);;All Files (*)"));

    if (fileName.isEmpty())
            return;

    this->app.Load(fileName.toStdString());

    this->currentFilename = fileName.toStdString();

}

void MainWindow::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save As"), this->currentFilename.c_str(),
        tr("Primordial Life Files (*.plfc, *.plfj);;All Files (*)"));

    if (fileName.isEmpty())
            return;

    this->currentFilename = fileName.toStdString();
    QFileInfo fi(this->currentFilename.c_str());
    QString compSuffix = fi.completeSuffix();
    if(compSuffix == "")
        this->currentFilename += ".plfc";

    this->app.Save(this->currentFilename);
}

void MainWindow::on_actionNew_triggered()
{
    QRect rect(0, 0, 2000, 1500);
    qint64 seed = QDateTime::currentMSecsSinceEpoch();
    int numBiots = this->app.env.settings.m_initialPopulation;

    this->app.env.DeleteContents();
    this->app.env.OnNew(*this->ui->openGLWidget, rect, numBiots, seed,
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

    app.Save(this->currentFilename);
}

void MainWindow::on_actionStart_Simulation_triggered()
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
    class SettingsUi settingsUi(app.env.settings);
    settingsUi.exec();
    int ret = settingsUi.result();
    if(ret == QDialog::Accepted)
    {
        app.sidesManager.updateListenMode();
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

void MainWindow::on_actionNetwork_Status_triggered()
{
    class NetworkUi networkUi(app.sidesManager);
    networkUi.exec();
}

void MainWindow::SelectedBiot(uint32_t biotId)
{
    if(!this->isFullScreen())
        this->ui->dockViewBiot->setVisible(true);
}

void MainWindow::on_actionFull_Screen_triggered()
{
    this->showFullScreen();
    this->ui->statusbar->setVisible(false);
    this->ui->menubar->setVisible(false);
    this->ui->dockViewBiot->setVisible(false);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_Escape and this->isFullScreen())
        {
            ExitFullscreen();
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::ExitFullscreen()
{
    showNormal();
    this->ui->statusbar->setVisible(true);
    this->ui->menubar->setVisible(true);
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    app.closeEvent(ev);
}

