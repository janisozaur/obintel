#ifndef EUCLIDEAN_H
#define EUCLIDEAN_H
#include <QList>
#include "distance.h"

class Euclidean : public Distance
{
public:
	Euclidean();
	double calculate(const QList<double>& centroid, const QList<double>& values);
};

#endif // EUCLIDEAN_H
