#ifndef MICROANALIZER_H
#define MICROANALIZER_H
#include "microarray.h"
#include "distance.h"
#include <QList>
#include <QPair>

typedef QPair< QList<int>, QList< QList<double> > > Cluster;

class Kmeans
{
public:
	Kmeans();
	~Kmeans();
    //uruchamia algorytm k-srednich
	void run(MicroArray* microArray);
    int _k; //liczba glup
    int _iterations; //liczba iteracji
	Distance* _distance;
    QList< QList<double> > _lastCentroids; //centroidy z ostatniej iteracji
    QList<int> _numberInCentr;//liczba wierszy macierzy przypisana do danego centroidu/grupy
    //QList<int> _classIDs; // deprecated:)
    QVector<int> _rowsClasses; //indeksy grup do ktorych zostaly przypisane kolejne wiersze macierzy

    //wybiera reprezentantow grup
    QList<int> picUpRepresentatives(MicroArray* microArray);
private:

    bool compareDistance(double distance1, double distance2);
};

#endif // MICROANALIZER_H
