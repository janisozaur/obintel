#include "microarray.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>

MicroArray::MicroArray(){
	_numbersOfGroups = 4;
	_numbersInGroups = 21;
	_startGroup = 0;
}

MicroArray::~MicroArray(){
}


void MicroArray::parseTxtFile(QString fileName){
	QFile file(fileName);
	if(!file.open(QFile::ReadOnly | QFile::Text)){
		qDebug() << "Nie mozna otworzyc pliku " + fileName;
		return;
	}
	QTextStream input(&file);
	QString line;
	while(!input.atEnd()){
		line = input.readLine();
		if(line.startsWith("!series_matrix_table_begin")){
			readMatrix(input);
			break;
		}
	}

	if(file.isOpen()){
		file.close();
	}
}

void MicroArray::readMatrix(QTextStream &input){
	//QString line = input.readLine();
	readIndividualLabels(input);
	readGeneValues(input);
}

void MicroArray::readIndividualLabels(QTextStream &input){
	QString line = input.readLine();
	QStringList sline = line.split("\t");
	sline.takeFirst();
	for(int i = _numbersInGroups * _startGroup ; i < _numbersInGroups * _numbersOfGroups; ++i){
		_individualLabels.append(sline.at(i));
	}
}

void MicroArray::readGeneValues(QTextStream &input){
	QString line = input.readLine();
	QStringList sline;
    QList<double> values;
	while(!line.startsWith("!series_matrix_table_end")){
		sline = line.split("\t");
        values.clear();
		_geneLabels.append(sline.takeFirst());
		for(int i = _numbersInGroups * _startGroup ; i < _numbersInGroups * _numbersOfGroups; ++i){
            values.append(sline.at(i).toDouble());
		}

		_matrix.append(values);
		line = input.readLine();
	}
}
