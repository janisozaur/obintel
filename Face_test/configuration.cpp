#include "configuration.h"

Configuration::Configuration()
{
}

Configuration::Configuration(QDir directory, QSize faceSize, QString netPath, QString netPath2):
	_directory(directory),
	_faceSize(faceSize),
	_netPath(netPath),
	_netPath2(netPath2)
{
}

Configuration::Configuration(Configuration &cfg){
    _directory = cfg.getDirectory();
    _faceSize = cfg.getFaceSize();
}

QSize Configuration::getFaceSize(){
    return _faceSize;
}

QDir Configuration::getDirectory(){
    return _directory;
}

QString Configuration::getNetPath(){
	return _netPath;
}

QString Configuration::getNetPath2(){
	return _netPath2;
}
