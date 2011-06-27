#ifndef PEARSON_H
#define PEARSON_H
#include "distance.h"
#include <QList>

class Pearson : public Distance
{
public:
	Pearson();
	double calculate(const QList<double>& centroid, const QList<double>& values);
};

#endif // PEARSON_H
