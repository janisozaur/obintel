#include "euclidean.h"
#include <math.h>
#include <QDebug>

Euclidean::Euclidean(){
	_type = "E";
}

double Euclidean::calculate(const QList<double>& centroid, const QList<double>& values){
	double result = 0.0;
	for(int i = 0; i < centroid.size(); ++i){
		result += (centroid[i] - values[i]) * (centroid[i] - values[i]);
	}
	return sqrt(result);
}
