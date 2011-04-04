#include "imagescanner.h"
#include <QDebug>

//TODO wczytac z konfiguracji
ImageScanner::ImageScanner(Configuration *cfg): _cfg(cfg),
	_minImageSize(_cfg->getFaceSize()),
	_imageScaleStep(32,32),//ta wartość jest dobrana na oko, sprawdzić
	_startFaceBoxPoint(0,0),
	_faceBoxPointStep(32,32)
{
    //...
}

//iteracja po obrazach znajdujących się w katalogu
void ImageScanner::scannDir(){
	QFileInfoList fileList = _cfg->getDirectory().entryInfoList(QDir::Files);
	foreach(QFileInfo fileInfo, fileList){
		QImage image(fileInfo.absolutePath());
		//przekazać do skalowania
	}
}

//przeskalowanie obrazka
//wyciecie kwadracika
//sprawdzenie za pomocą nn
//powtorzenie operacji dla innego kwadracika
QList<QRect> ImageScanner::scannPicture(const QImage &image){
    QList<QRect> faces;
	QRect faceBox(_startFaceBoxPoint, _cfg->getFaceSize());
	for(QPoint faceBoxMovePoint = _startFaceBoxPoint;
		faceBoxMovePoint.x() < image.width();
		faceBoxMovePoint.setX(faceBoxMovePoint.x() + _faceBoxPointStep.x()))
	{

		faceBoxMovePoint.setY(0);
		faceBox.moveTo(faceBoxMovePoint);

		for(;faceBoxMovePoint.y() < image.height();
			faceBoxMovePoint.setY(faceBoxMovePoint.y() + _faceBoxPointStep.y()))
		{
			faceBox.moveTo(faceBoxMovePoint);
			//jeśli twarz to dodany
			if(checkRect(image.copy(faceBox))){
				faces.append(faceBox);
			}
		}
	}
    return faces;
}



bool ImageScanner::checkRect(const QImage &faceBox){
    //tu przygotować face dla sieci i zwrócić 1 albo 0 z tej sieci;
    return true;
}

QList<QRect> ImageScanner::scaleImage(const QImage &image){
	QList<QRect> faces;
	for(QSize size = _minImageSize;
		size.width() <= image.size().width();
		size = size + _imageScaleStep){

		faces.append(scannPicture(image.scaled(size)));
	}
	return faces;
}
