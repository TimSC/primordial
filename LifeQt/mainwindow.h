#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
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

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_As_triggered();

    void on_actionNew_triggered();

    void on_actionExit_triggered();

    void on_actionSave_triggered();

    void on_actionStart_Simulation_triggered();

    void on_actionLearn_about_Primordial_Life_triggered();

    void on_actionStart_Simulation_triggered(bool checked);

private:
    Ui::MainWindow *ui;

    class Environment env;
    std::string currentFilename;

    QGraphicsScene scene;
};
#endif // MAINWINDOW_H
