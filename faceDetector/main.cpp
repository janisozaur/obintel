#include <QtCore/QCoreApplication>

#include "parameterreader.h"

//usunieto normalizacje przy zapisywaniu danych treningowych i przy testowaniu(wszedzie gdzie obrazek zamieniano na wektor floatow)

// PARAMETRY
// generowanie zbioru treningowego: gen plik_do_zapisu katalog_ze_zdjeciami (uwaga, zbior walidacyjny trzeba wyciac ze zbioru treningowego, program nie robi tego automatycznie)
// uczenie: train plik_zapisu_sieci zbior_treningowy zbior_walidacyjny  wspolczynnik_nauki momentum
// testowanie: test plik_z_siecia1 plik_z_siecia2 katalog_testowy prog_testu

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    ParameterReader reader;

    reader.readParameters();

    return 0;
}
