#ifndef DISTANCE_H
#define DISTANCE_H
#include <QList>
#include <QString>

class Distance{
public:
	virtual double calculate(const QList<double>& centroid, const QList<double>& values) = 0;
	virtual QString type(){return _type;}
protected:
	QString _type;
};

#endif // DISTANCE_H
