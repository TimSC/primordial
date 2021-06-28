#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QLineEdit>
#include "core/Environ.h"
#include "rapidjson/document.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void timerEvent(QTimerEvent *event);

    void updateToolMenu();

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_As_triggered();

    void on_actionNew_triggered();

    void on_actionExit_triggered();

    void on_actionSave_triggered();

    void on_actionStart_Simulation_triggered();

    void on_actionLearn_about_Primordial_Life_triggered();

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

private:
    Ui::MainWindow *ui;
    QLineEdit *statusDay, *statusPopulation, *statusExtinctions;

    class Environment env;
    std::string currentFilename;
    std::string currentTool;

    int64_t lastSimUpdate;
    int64_t lastGraphicsUpdate;
    int64_t lastStatsUpdate;

};
#endif // MAINWINDOW_H
