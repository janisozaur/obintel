#include "configuration.h"

Configuration::Configuration()
{
}

Configuration::Configuration(QDir directory, QSize faceSize){
    _directory = directory;
    _faceSize = faceSize;
}

void Configuration::setDirectory(QDir directory){
    _directory = directory;
}

void Configuration::setFaceSize(QSize faceSize){
    _faceSize = faceSize;
}

QSize Configuration::getFaceSize() const{
    return _faceSize;
}

QDir Configuration::getDirectory() const{
    return _directory;
}
