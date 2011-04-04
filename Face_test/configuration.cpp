#include "configuration.h"

Configuration::Configuration()
{
}

Configuration::Configuration(QDir directory, QSize faceSize){
    _directory = directory;
    _faceSize = faceSize;
}

Configuration::Configuration(Configuration &cfg){
    _directory = cfg.getDirectory();
    _faceSize = cfg.getFaceSize();
}

void Configuration::setDirectory(QDir directory){
    _directory = directory;
}

void Configuration::setFaceSize(QSize faceSize){
    _faceSize = faceSize;
}

QSize Configuration::getFaceSize(){
    return _faceSize;
}

QDir Configuration::getDirectory(){
    return _directory;
}
