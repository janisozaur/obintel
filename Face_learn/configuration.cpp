#include "configuration.h"

Configuration::Configuration()
{
}

Configuration::Configuration(QDir facesDirectory, QDir nonFacesDirectory, QSize faceSize,
							 QString dataSavePath, QString netSavePath):
	_facesDirectory(facesDirectory),
	_nonFacesDirectory(nonFacesDirectory),
	_faceSize(faceSize),
	_dataSavePath(dataSavePath),
	_netSavePath(netSavePath)
{
}

QSize Configuration::getFaceSize() const{
    return _faceSize;
}

QDir Configuration::getFacesDirectory() const{
	return _facesDirectory;
}

QDir Configuration::getNonFacesDirectory() const{
	return _nonFacesDirectory;
}

QString Configuration::getDataSavePath() const{
	return _dataSavePath;
}

QString Configuration::getNetSavePath() const{
	return _netSavePath;
}

