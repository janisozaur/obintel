#include "kmeans.h"
#include <QDebug>
#include <QMap>
#include <cmath>
#include <ctime>

Kmeans::Kmeans()
{
    this->_k = 20;
    this->_iterations = 20;
}

Kmeans::~Kmeans(){
	delete _distance;
}

//implementacja metody k-średnich
void Kmeans::run(MicroArray* microArray){

	//Centroidy to wiersze wokół których formują się grupy. Pierwsze centroidy pochodzą
    //z macierzy ale w kolejnych iteracjach ich wartości są wyliczane jako średnie grupy
	QList< QList<double> > centroids;
    _rowsClasses.resize(microArray->_matrix.size());
    qDebug() << "ilosc sierszy w rowsClasses: " << _rowsClasses.size();

	//Wylosowanie pierwszego zestawu centroidów z macierzy
	QList<int> tempIds;
	while(_lastCentroids.size() < _k){
		int id = floor(rand() % microArray->_matrix.size());
		if(!tempIds.contains(id)){
			tempIds.append(id);
            _lastCentroids.append(microArray->_matrix[id]);
		}
	}

	//pętla iteracji metody
	for(int i = 0; i < _iterations; ++i){

		//wyzerowanie liczebnosci klastrów i ustawienie nowych centroidów
        //_classIDs.clear();
		centroids.clear();
		centroids = _lastCentroids;
		_numberInCentr.clear();

		for(int i = 0; i < _k; ++i){
			_numberInCentr.append(0);
			for(int j = 0; j < centroids.first().size(); ++j){
				_lastCentroids[i][j] = 0.0;
			}
		}

		//pętla wierszy macierzy
		for(int j = 0; j < microArray->_matrix.size(); ++j){
            double bestDistance = _distance->calculate(centroids[0], microArray->_matrix[j]);
			double distance = bestDistance;
			int centrID = 0;//id centroidu dla którego dystans do aktualnego wiersza był najmniejszy
			//pętla centroidów
			for(int k = 1; k < _k; ++k){
                distance = _distance->calculate(centroids[k], microArray->_matrix[j]);

                if(compareDistance(distance, bestDistance)){
					centrID = k;
					bestDistance = distance;
                }
			}

            _numberInCentr[centrID] += 1;//inkrementacja liczebnosci grupy

            //_classIDs.append(centrID);
            _rowsClasses[j] = centrID; //przypisanie wiersza o ID=j do clasy centrID

			//dodanie wartości z nowego wiersza(przyda się przy liczeniu średniej)
			for(int k = 0; k < _lastCentroids.first().size(); ++k){
                _lastCentroids[centrID][k] += microArray->_matrix[j][k];
			}
		}

		//obliczenie nowych centroidów
		for(int j = 0; j < _k; ++j){
			if(_numberInCentr[j] != 0){
				for(int k = 0; k < _lastCentroids[j].size(); ++k){
					_lastCentroids[j][k] = _lastCentroids[j][k] / _numberInCentr[j];
				}
			}
		}
	}

}


bool Kmeans::compareDistance(double distance1, double distance2){

    //im mniejsza odległość euklidesowa tym lepiej
    if(_distance->type() == "E" && distance1 < distance2){

       return true;

    //im większa corelacja tym lepiej
    } else if(_distance->type() == "C" && distance1 > distance2){

       return true;

    }

    return false;
}

QList<int> Kmeans::picUpRepresentatives(MicroArray* microArray){

    QList<int> representatives;
    double distance, bestDistance;
    int bestID = 0;

    for(int i = 0; i < this->_lastCentroids.size(); ++i){
        bestDistance = _distance->calculate(_lastCentroids[i], microArray->_matrix[0]);//!!! to moze nie dzialac

        for(int j = 0; j < this->_rowsClasses.size(); ++j){

            if(_rowsClasses[j] == i){

                distance = _distance->calculate(_lastCentroids[i], microArray->_matrix[j]);

                if(compareDistance(distance, bestDistance)){
                    bestID = j;
                    bestDistance = distance;
                }
            }
        }

        representatives.append(bestID);
    }
    return representatives;
}
