#include <QtCore/QCoreApplication>

#include "parameterreader.h"


//PARAMETRY: katalog_orginalny katalog_z_wynikami plik_wyniku_analizy prog
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ParameterReader reader;

    reader.readParameters();

    return 0;
}
