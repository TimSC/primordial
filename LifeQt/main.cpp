#include <QCoreApplication>
#include <QDateTime>
#include <iostream>
#include "Environ.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    class Environment env;
    QRect rect(0, 0, 1080, 760);

    env.options.Reset(640, 480);

    env.OnNew(rect, 20, QDateTime::currentMSecsSinceEpoch(),
                0, 1, 10);

    while (true)
    {
        cout << env.GetPopulation() << endl;
        env.Skip();
    }

    return a.exec();
}
