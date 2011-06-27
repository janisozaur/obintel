#include "pearson.h"
#include <QList>
#include <cmath>

Pearson::Pearson(){
	_type = "C";
}

double Pearson::calculate(const QList<double>& centroid, const QList<double>& values){
	double avgCentr = 0.0;
	double avgVal = 0.0;

	for(int i = 0; i < centroid.size(); ++i){
		avgCentr += centroid.at(i);
		avgVal += values.at(i);
	}

	avgCentr /= (double)centroid.size();
	avgVal /= (double)values.size();

	double nom = 0.0;
	double denX = 0.0;
	double denY = 0.0;

	for(int i = 0; i < centroid.size(); ++i){
		double foo = centroid.at(i) - avgCentr;
		double bar = values.at(i) - avgVal;
		nom += foo * bar;
		denX += foo * foo;
		denY += bar * bar;
	}

	return std::abs(-nom / (sqrt(denX) * sqrt(denY)));
}
