#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QLineEdit>
#include "core/Environ.h"
#include "networking.h"
#include "core/json.h"
#include "rapidjson/document.h"
#include "dockviewbiot.h"
#include "environmentarea.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainApp : public QObject
{
    Q_OBJECT
public:
    MainApp();
    virtual ~MainApp();

    class Environment env;

    int64_t lastSimUpdate;
    int64_t lastFuzz;

    class SidesManager sidesManager;
    class AutoConnect autoConnect;

    void TimedUpdate(bool running);
    void timerEvent(QTimerEvent *event) override;

    void closeEvent(QCloseEvent *bar);

    void Save(const std::string &filename);
    void Load(const std::string &filename);

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void timerEvent(QTimerEvent *event);

    void updateToolMenu();

public slots:
    void SelectedBiot(uint32_t biotId);

    void ExitFullscreen();

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_As_triggered();

    void on_actionNew_triggered();

    void on_actionExit_triggered();

    void on_actionSave_triggered();

    void on_actionStart_Simulation_triggered();

    void on_actionStart_Simulation_triggered(bool checked);

    void on_actionCure_Sicken_triggered(bool checked);

    void on_actionExamine_triggered(bool checked);

    void on_actionFeed_triggered(bool checked);

    void on_actionMutate_triggered(bool checked);

    void on_actionOpenSingle_triggered(bool checked);

    void on_actionRelocate_triggered(bool checked);

    void on_actionSaveSingle_triggered(bool checked);

    void on_actionTerminate_triggered(bool checked);

    void on_actionSettings_triggered();

    void on_actionStatus_Bar_triggered(bool checked);

    void on_actionAbout_triggered();

    void on_actionNetwork_Status_triggered();

    void on_actionFull_Screen_triggered();

protected:

    bool eventFilter(QObject *obj, QEvent *event);

private:
    void closeEvent(QCloseEvent *bar);

    Ui::MainWindow *ui;
    QLineEdit *statusDay, *statusPopulation, *statusExtinctions, *statusNetwork;

    std::string currentFilename;
    std::string currentTool;

    int64_t lastGraphicsUpdate;
    int64_t lastStatsUpdate;

    class MainApp app;

};
#endif // MAINWINDOW_H
