#ifndef SPEARMAN_H
#define SPEARMAN_H
#include "distance.h"

class Spearman : public Distance
{
public:
	Spearman();
	double calculate(const QList<double>& centroid, const QList<double>& values);;
};

#endif // SPEARMAN_H
