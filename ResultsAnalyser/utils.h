#ifndef UTILS_H
#define UTILS_H
#include "configuration.h"

#include <QList>
#include <QRect>
#include <QPair>

class Utils
{
public:
    static void analyseResults(Configuration &cfg);

private:

    //pobiera wspolrzedne twarzy z pliku txt
    static QList<QRect> getCoords(QString fileName);
    //oblicza pole
    static double calcArea(const QRect &rect);
    //sprawdza czy dwa prostokaty maja wspolny obszar o powierzchni przekraczajacej pewien prog
    static bool compareRects(const QRect &faceRect, const QRect &resultRect, double threshold);
    //zapisuje wyniki do pliku tekstowego
    static void saveResults(QList< QPair<double, double> > result, QString resultFile);
    //funckcja zwraca procentowe wyniki, pierwszy element to procent znalezionych twarzy, drugi to ilosc falszywych wynikow
    static QPair<double, double> compareCoords(const QList<QRect> &orginalCoords, const QList<QRect> &resultsCoords, double threshold);
    Utils();
};

#endif // UTILS_H
