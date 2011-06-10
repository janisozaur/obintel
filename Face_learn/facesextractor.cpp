#include "facesextractor.h"
#include <QDebug>
#include <QPoint>
#include "cmath"

FacesExtractor::FacesExtractor() {
}

QList<QImage> FacesExtractor::getFaces(QSize size, QDir dir) {
    QList<QImage> imageList;
	dir.setFilter(QDir::Files);
	QFileInfoList fileList = dir.entryInfoList();

	for(int i = 0; i < fileList.length(); i += 2){
		qDebug() << fileList.at(i).absoluteFilePath();
        QImage image(fileList.at(i).absoluteFilePath());
        if(image.isNull()){
			qWarning() << "Nie można otworzyć zdjecia: " + fileList.at(i).absoluteFilePath();
            continue;
        }
        QFile coordFile(fileList.at(i+1).absoluteFilePath());
        QList<QRect> facesCoords = getFacesCoords(coordFile);

        foreach(QRect rect , facesCoords){
            QImage face = image.copy(rect);
            if(!face.isNull()){
				face = face.scaled(size);
				//histogramEqualization(&face);
				imageList.append(face);
            }
        }
    }
	qDebug() << "Przetworzono zdecia z katalogu: " + dir.absolutePath();
    return imageList;
}

//funkcja oczekuje obrazków z samymi twarzami, nie potrzebuje coordynatów twarzy
QList<QImage> FacesExtractor::getFacesWithoutCoords(QSize size, QDir dir){
	QList<QImage> imageList ;
	dir.setFilter(QDir::Files);
	QFileInfoList fileList = dir.entryInfoList();

	for(int i = 0; i < fileList.length(); ++i){
		qDebug() << fileList.at(i).absoluteFilePath();
		QImage image(fileList.at(i).absoluteFilePath());
		if(image.isNull()){
			qWarning() << "Nie można otworzyć zdjecia: " + fileList.at(i).absoluteFilePath();
			continue;
		}
		image = image.scaled(size);
		histogramEqualization(&image);
		imageList.append(image);
	}
	qDebug() << "Przetworzono zdecia z katalogu: " + dir.absolutePath();
	return imageList;
}

void FacesExtractor::saveFacesToFile(QString filePath, const QList<QImage> &facesList, const QList<QImage> &nonFacesList, int inputs){
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
			 qWarning() << "Nie można otworzyć pliku do zapisu danych treningowych";
			 return;
	}
	QTextStream outputStream(&file);
	outputStream << facesList.size() + nonFacesList.size() << ' ' << inputs << ' ' << 1 << "\n";
	foreach(QImage img, facesList){
		QImage image = img.convertToFormat(QImage::Format_RGB32);
		QRgb *tab = (QRgb*)image.bits();
		for(int i = 0; i < inputs; ++i){
			outputStream << qRed(*tab)/255.0 << ' ';//TODO czy normalizowac do [0,1]
			++tab;
		}
		outputStream << "\n" << 1 << "\n";
	}
	foreach(QImage img, nonFacesList){
		QImage image = img.convertToFormat(QImage::Format_RGB32);
		QRgb *tab = (QRgb*)image.bits();
		for(int i = 0; i < inputs; ++i){
			outputStream << qRed(*tab)/255.0 << ' ';//TODO czy normalizowac do [0,1]
			++tab;
		}
		outputStream << "\n" << -1 << "\n";
	}
	file.close();

}

QList<QRect> FacesExtractor::getFacesCoords(QFile &coordFile) {
    QList<QRect> coords;
    if (!coordFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "Nie można otworzyć pliku z konfiguracją: " + coordFile.fileName();
        return coords;
    }
    QTextStream coordStream(&coordFile);
    while(!coordStream.atEnd()){
        QString faceCoord = coordStream.readLine();
        QStringList coordList = faceCoord.split(" ");
        QPoint topLeft(coordList.at(0).toInt(), coordList.at(1).toInt());
        QPoint bottomRight(coordList.at(2).toInt(), coordList.at(3).toInt());
        QRect faceRect(topLeft, bottomRight);
        coords.append(faceRect);
    }
    coordFile.close();
    return coords;
}

void FacesExtractor::histogramEqualization(QImage *img){
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
