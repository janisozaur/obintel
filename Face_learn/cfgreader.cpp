#include "cfgreader.h"
#include <QDebug>
#include <QTextStream>

//format pliku
//katalog faces
//katalog nonfaces
//rozmiar do jakiego skaluje
//sciezka do zapisu danych treningowych
//sciezka do zapisu konfiguracji perceptronu
Configuration* CfgReader::readConfiguration(QString cfgPath){

    QFile file(cfgPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
			 qWarning() << "Nie mozna otworzyc pliku z konfiguracja";
    }
    QTextStream stream(&file);
	QDir facesdir(stream.readLine());
	QDir nonFacesDir(stream.readLine());
    QStringList strList = stream.readLine().split(" ");
    QSize size(strList.at(0).toInt(), strList.at(1).toInt());
	QString dataSavePath(stream.readLine());
	QString netSavePath(stream.readLine());
    file.close();

	return new Configuration(facesdir, nonFacesDir, size, dataSavePath, netSavePath);
}

CfgReader::CfgReader()
{
}
