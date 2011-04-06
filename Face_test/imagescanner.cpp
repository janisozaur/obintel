#include "imagescanner.h"
#include "floatfann.h"
#include "fann_cpp.h"
#include <QDebug>
#include <QVector>

//TODO wczytac z konfiguracji
ImageScanner::ImageScanner(Configuration *cfg): _cfg(cfg),
	_minImageSize(_cfg->getFaceSize()),
	_imageScaleStep(32,16),//ta wartość jest dobrana na oko, sprawdzić
	_startFaceBoxPoint(0,0),
	_faceBoxPointStep(16,8)
{

	_net.create_from_file(_cfg->getNetPath().toStdString());
	_net2.create_from_file(_cfg->getNetPath2().toStdString());
    //...
}

//iteracja po obrazach znajdujących się w katalogu
void ImageScanner::scannDir(){
	QFileInfoList fileList = _cfg->getDirectory().entryInfoList(QDir::Files);
	QList<QRect> faceBoxes;
	foreach(QFileInfo fileInfo, fileList){
		QImage image(fileInfo.absoluteFilePath());
		qDebug() << fileInfo.absoluteFilePath();
		faceBoxes = scaleImage(image);

		QFile file(fileInfo.absolutePath()+"/"+fileInfo.baseName()+".txt");
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
			qWarning() << "Nie można zapisac wynikow";
		}
		QTextStream stream(&file);
		foreach(QRect rect, faceBoxes){
			stream << rect.topLeft().x() << " " << rect.topLeft().y() << " "
					  << rect.bottomRight().x() << " " << rect.bottomRight().y() << "\n";
		}
		file.close();
	}
}

//przeskalowanie obrazka
//wyciecie kwadracika
//sprawdzenie za pomocą nn
//powtorzenie operacji dla innego kwadracika
QList<QRect> ImageScanner::scannPicture(const QImage &scaledImage, const QImage &orginalImage){
    QList<QRect> faces;
	QRect faceBox(_startFaceBoxPoint, _cfg->getFaceSize());

	for(QPoint faceBoxMovePoint = _startFaceBoxPoint;
		faceBoxMovePoint.x() < scaledImage.width();
		faceBoxMovePoint.setX(faceBoxMovePoint.x() + _faceBoxPointStep.x()))
	{
		faceBoxMovePoint.setY(0);
		faceBox.moveTo(faceBoxMovePoint);

		for(;faceBoxMovePoint.y() < scaledImage.height();
			faceBoxMovePoint.setY(faceBoxMovePoint.y() + _faceBoxPointStep.y()))
		{
			faceBox.moveTo(faceBoxMovePoint);

			//jeśli twarz to dodany
			if(checkRect(scaledImage.copy(faceBox))){
				//skalowanie
//				static int f = 0;
//				scaledImage.copy(faceBox).save(QString("D:/duzy-%1.png").arg(f++));
				int topLeftX = faceBox.topLeft().x()*orginalImage.width()/scaledImage.width();
				int topLeftY = faceBox.topLeft().y()*orginalImage.height()/scaledImage.height();
				int width = faceBox.width()*orginalImage.width()/scaledImage.width();
				int height = faceBox.height()*orginalImage.height()/scaledImage.height();
				QRect box(topLeftX, topLeftY, width, height);
				faces.append(box);
			}
		}
	}
    return faces;
}



bool ImageScanner::checkRect(const QImage &face){
	//przerobienie obrazka na tablice float
	QImage image = face.convertToFormat(QImage::Format_RGB32);

	histogramEqualization(&image);

	QRgb *tab = (QRgb*)image.bits();
	float *pixels = new float[face.width()*face.height()];
	for(int i = 0; i < face.width()*face.height(); ++i){

		pixels[i] = qRed(*tab)/255.0;
		++tab;
	}
	float result = _net.run(pixels)[0];//odpalenie sieci neuronowej dla obrazka
	float result2 = _net2.run(pixels)[0];
	if((result+result2)/2.0 > 0.999){
		delete []pixels;
		return true;
	} else {
		delete []pixels;
		return false;
	}
}

QList<QRect> ImageScanner::scaleImage(const QImage &image){
	QList<QRect> faces;
	QImage img = image.scaled(768, 384);

	for(QSize size = img.size();// = _minImageSize;
		size.width() > 64;
		size -= _imageScaleStep)
	{

		faces.append(scannPicture(image.scaled(size), image));

	}
	return faces;
}

void ImageScanner::histogramEqualization(QImage *img){
	QVector<int> histogram;
	histogram.fill(0,256);

	QRgb pixel;
	for( int x = 0; x < img->width(); x++ ){
		for( int y = 0; y < img->height(); y++ ){
			pixel = img->pixel( x, y );
			histogram[qRed(pixel)]++;
		}
	}
	QVector<float> dystrybuanta;
	dystrybuanta.fill(0,256);

	for( int i = 0; i < 256; i++ ){
		int tmp = 0;
		for( int j = 0; j <= i; j++ ){
			tmp += histogram.at(j);
		}
		dystrybuanta[i] = double(tmp) / double((img->width()*img->height()));
	}

	double d0 = 0.0;
	for( int i = 0; i < dystrybuanta.count(); i++ ){
		if( dystrybuanta.at(i) > 0 ){
			d0 = dystrybuanta.at(i);
			break;
		}
	}
	QVector<int> lut;
	lut.fill( 0,256 );
	for( int i = 0; i < 256; i++ ){
		lut[i] = floor( ((dystrybuanta.at(i) - d0)/ (1.0 - d0))*(256.0-1.0));
	}

	QVector<QRgb> colorTable = img->colorTable();
	for( int i = 0; i < colorTable.count(); i++ ){
		QRgb pixel = colorTable.at(i);
		pixel = qRgb( lut[qRed(pixel)], lut[qRed(pixel)],lut[qRed(pixel)]);
		colorTable.replace(i,pixel);
	}
	img->setColorTable( colorTable );
}
