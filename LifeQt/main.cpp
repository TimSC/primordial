#include "mainwindow.h"

#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const bool fuzzEnabled = a.arguments().contains("--fuzz");
#if 1
    MainWindow w(fuzzEnabled);
    w.show();
#else
    //Run with no graphics or UI
    //MainApp app;
    //while(1)
    //    QThread::sleep(1000);

#endif
    return a.exec();
}
