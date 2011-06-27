#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QTime>
#include <QtGlobal>
#include <ctime>
#include "parameterreader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTime t;
    srand(t.currentTime().msec());

    ParameterReader reader;
    reader.readParameters();


    return 0;
}
