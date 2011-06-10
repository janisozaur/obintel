#include "utils.h"

#include <QDebug>
#include <QFileInfoList>
#include <QDir>
#include <QImage>

Utils::Utils()
{
}


void Utils::trainNetwork(Configuration& cfg)
{
	setlocale(LC_ALL, "C");

	struct fann *ann = fann_create_standard(3, cfg._numInput, cfg._numHidden, cfg._numOutput);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_steepness_hidden(ann, 0.5);
	fann_set_activation_steepness_output(ann, 0.5);
	fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);
	fann_set_learning_rate(ann, cfg._learningRate);
	fann_set_learning_momentum(ann, cfg._momentum);
	fann_set_callback(ann, printCallback);

	fann_train_data *valData = fann_read_train_from_file(cfg._valDataFile.toStdString().c_str());

	float lastMSE = 1000000.0;//zmienic na max double
	float currentMSE;

	for(int i = 0; i < cfg._totalEpochs / cfg._epochsBetweenReports; ++i){

		//co pewna liczbe epok bedzie wykonywane testowanie sieci na zbiorze walidacyjnym
		fann_train_on_file(ann, cfg._dataFile.toStdString().c_str(), cfg._epochs, cfg._epochsBetweenReports, cfg._desiredError);

		//testowanie na zbiorze walidacyjnym
		currentMSE = fann_test_data(ann, valData);
		qDebug() << "current MSE:\t" << currentMSE;

		//jesli blad na zbiorze walidacyjnym zaczyna rosnac przerywamy uczenie.
		if(currentMSE > lastMSE + cfg._valThreshold){

			break;
		}

		lastMSE = currentMSE;
	}

	//zapisanie ostatniej sieci
	fann_save(ann, cfg._annSaveFile.toStdString().c_str());
}


// Callback function that simply prints the information to log file(from documentation)
int Utils::printCallback(struct fann *net, struct fann_train_data *train,
		unsigned int max_epochs, unsigned int epochs_between_reports,
		float desired_error, unsigned int epochs)
{
	//zapisac do pliku
	QFile file("mse.log");
	if (!file.open(QIODevice::Append | QIODevice::Text)){
			 qWarning() << "Nie mozna otworzyc pliku do logowania";
	}
	QTextStream stream(&file);
	stream << fann_get_MSE(net) << endl;
	file.close();
	return 0;
}


void Utils::testNetwork(Configuration &cfg){

	struct fann *ann = fann_create_from_file(cfg._annSaveFile.toStdString().c_str());
	struct fann *ann2 = fann_create_from_file(cfg._annSaveFile2.toStdString().c_str());

	QDir dir(cfg._directory);
	QStringList filters;
	filters << "*.png" << "*.jpg";
	dir.setNameFilters(filters);
	QFileInfoList fileList = dir.entryInfoList(QDir::Files);
	QList<QRect> faceBoxes;

	foreach(QFileInfo fileInfo, fileList){

		QImage image(fileInfo.absoluteFilePath());
		qDebug() << fileInfo.absoluteFilePath();

		faceBoxes = scaleImage(image, cfg, ann, ann2);

		QFile file(fileInfo.absolutePath()+"/"+fileInfo.baseName()+".txt");

		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){

			qWarning() << "Nie mozna zapisac wynikow";
		}

		QTextStream stream(&file);
		foreach(QRect rect, faceBoxes){

			stream << rect.topLeft().x() << " " << rect.topLeft().y() << " "
					  << rect.bottomRight().x() << " " << rect.bottomRight().y() << "\n";
		}

		file.close();
	}
}

QList<QRect> Utils::scaleImage(const QImage &image, Configuration &cfg, struct fann *ann, struct fann *ann2){

	QList<QRect> faces;

	for(QSize size = cfg._orginalImageSize; // = _minImageSize;
		size.width() > cfg._minImageSize.width();
		size -= cfg._imageScaleStep)
	{
		//scannImage(image.scaled(size), cfg, ann);
		faces.append(scannImage(image.scaled(size), cfg, ann, ann2));
	}

	return faces;
}

QList<QRect> Utils::scannImage(const QImage &image, Configuration &cfg, struct fann *ann, struct fann *ann2){

	QList<QRect> faces;

	QRect faceBox(cfg._startFaceBoxPoint, cfg._faceSize);

	for(QPoint faceBoxMovePoint = cfg._startFaceBoxPoint;
		faceBoxMovePoint.x() < image.width() - cfg._faceSize.width();
		faceBoxMovePoint.setX(faceBoxMovePoint.x() + cfg._faceBoxPointStep.x()))
	{
		faceBoxMovePoint.setY(0);
		faceBox.moveTo(faceBoxMovePoint);

		for(;faceBoxMovePoint.y() < image.height() - cfg._faceSize.height();
			faceBoxMovePoint.setY(faceBoxMovePoint.y() + cfg._faceBoxPointStep.y()))
		{
			faceBox.moveTo(faceBoxMovePoint);

			//jesli twarz to dodajemy do listy
			if(checkRect(image.copy(faceBox), cfg, ann, ann2)){
				//skalowanie
//				static int f = 0;
//				scaledImage.copy(faceBox).save(QString("D:/duzy-%1.png").arg(f++));
				int topLeftX = faceBox.topLeft().x() * cfg._orginalImageSize.width() / image.width();
				int topLeftY = faceBox.topLeft().y() * cfg._orginalImageSize.height() / image.height();
				int width = faceBox.width() * cfg._orginalImageSize.width() / image.width();
				int height = faceBox.height() * cfg._orginalImageSize.height() / image.height();
				QRect box(topLeftX, topLeftY, width, height);
				faces.append(box);
			}
		}
	}
	return faces;
}

bool Utils::checkRect(const QImage &face, Configuration &cfg, struct fann *ann, struct fann *ann2){

	QImage img = face.convertToFormat(QImage::Format_RGB32);

	//histogramEqualization(&img);

	QRgb *tab = (QRgb*)img.bits();
	float *pixels = new float[face.width()*face.height()];

	for(int i = 0; i < face.width()*face.height(); ++i){

		pixels[i] = qRed(*tab) / 255.0;
		++tab;
	}

	float result = *fann_run(ann, pixels);
	float result2 = *fann_run(ann2, pixels);
	//qDebug() << "dd: " << result << "\t" << result2;

	if((result + result2)/2.0 > cfg._testThreshold){

		delete []pixels;
		return true;

	} else {

		delete []pixels;
		return false;
	}
}

void Utils::prepareData(Configuration &cfg){

	QList<QImage> faces = prepareFaces(cfg);
	QList<QImage> nonFaces = prepareNonFaces(cfg);
	saveTrainData(cfg, faces, nonFaces);
	qDebug() << "faces: " << faces.size() << "\t" << "nonFaces: " << nonFaces.size();
}

QList<QImage> Utils::prepareFaces(Configuration &cfg){

	QList<QImage> imageList;
	QDir dir(cfg._directory);
	dir.setFilter(QDir::Files);
	QFileInfoList fileList = dir.entryInfoList();

	for(int i = 0; i < fileList.length(); i += 2){

		QImage image(fileList.at(i).absoluteFilePath());
		if(image.isNull()){
			qWarning() << "Nie mozna odczytac zdjecia: " + fileList.at(i).absoluteFilePath();
			continue;
		}

		QList<QRect> facesCoords = getFacesCoords(fileList.at(i+1).absoluteFilePath());
		qDebug() << fileList.at(i).absoluteFilePath() << "\t liczba twarzy: " << facesCoords.size();

		foreach(QRect rect , facesCoords){

			QImage face = image.copy(rect);

			face = face.scaled(cfg._faceSize);

			if(!face.isNull()){
				//histogramEqualization(&face);
				imageList.append(face);
				//img.save(QString("D:/img-%1.png").arg(f++));
			}
		}
	}

	return imageList;
}

QList<QImage> Utils::prepareNonFaces(Configuration &cfg){

	QList<QImage> imageList;
	QDir dir(cfg._directory);
	dir.setFilter(QDir::Files);
	QFileInfoList fileList = dir.entryInfoList();

	for(int i = 0; i < fileList.length(); i += 2){

		QImage image(fileList.at(i).absoluteFilePath());
		if(image.isNull()){
			qWarning() << "Nie mozna odczytac zdjecia: " + fileList.at(i).absoluteFilePath();
			continue;
		}

		QList<QRect> facesCoords = getFacesCoords(fileList.at(i+1).absoluteFilePath());
		qDebug() << fileList.at(i+1).absoluteFilePath() << "\t liczba nie-twarzy: " << facesCoords.size()*4;

		foreach(QRect rect , facesCoords){

			QRect r1 = rect.translated(rect.width(), 0);
			QRect r2 = rect.translated(-rect.width(), 0);
			QRect r3 = rect.translated(0, rect.height());
			QRect r4 = rect.translated(0, -rect.height());

			QList<QRect> rects;
			rects.append(r1);
			rects.append(r2);
			rects.append(r3);
			rects.append(r4);

			QImage img;
			foreach(QRect r, rects){

				img = image.copy(r).scaled(cfg._faceSize);

				if(!img.isNull()){
					imageList.append(img);
					//img.save(QString("D:/img-%1.png").arg(f++));
				}
			}
		}
	}

	return imageList;
}


QList<QRect> Utils::getFacesCoords(QString coordsFile){

	QFile file(coordsFile);
	QList<QRect> coords;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		qWarning() << "Nie mozna odczytac pliku z koordynatami: " + coordsFile;
		return coords;
	}

	QTextStream coordStream(&file);

	while(!coordStream.atEnd()){

		QString faceCoord = coordStream.readLine();
		QStringList coordList = faceCoord.split(" ");
		QPoint topLeft(coordList.at(0).toInt(), coordList.at(1).toInt());
		QPoint bottomRight(coordList.at(2).toInt(), coordList.at(3).toInt());
		QRect faceRect(topLeft, bottomRight);
		coords.append(faceRect);
	}

	file.close();
	return coords;
}

void Utils::saveTrainData(Configuration &cfg, QList<QImage> faces, QList<QImage> nonFaces){

	QFile file(cfg._dataFile);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
			 qWarning() << "Nie można utworzyć pliku do zapisu danych treningowych";
			 return;
	}

	QTextStream outputStream(&file);
	outputStream << faces.size() + nonFaces.size() << ' ' << cfg._numInput << ' ' << cfg._numOutput << "\n";

	foreach(QImage img, faces){

		QImage image = img.convertToFormat(QImage::Format_RGB32);
		QRgb *tab = (QRgb*)image.bits();

		for(int i = 0; i < cfg._numInput; ++i){

			outputStream << qRed(*tab) / 255.0 << ' ';
			++tab;
		}

		outputStream << "\n" << 1 << "\n";
	}

	foreach(QImage img, nonFaces){

		QImage image = img.convertToFormat(QImage::Format_RGB32);
		QRgb *tab = (QRgb*)image.bits();

		for(int i = 0; i < cfg._numInput; ++i){

			outputStream << qRed(*tab) / 255.0 << ' ';
			++tab;
		}

		outputStream << "\n" << -1 << "\n";
	}

	file.close();
}

void Utils::histogramEqualization(QImage *img){
	QVector<int> histogram;
	histogram.fill(0,256);

	QRgb pixel;
	for( int x = 0; x < img->width(); x++ ){
		for( int y = 0; y < img->height(); y++ ){
			pixel = img->pixel( x, y );
			histogram[qRed(pixel)]++;
		}
	}
	QVector<float> distribution;
	distribution.fill(0,256);

	for( int i = 0; i < 256; i++ ){
		int tmp = 0;
		for( int j = 0; j <= i; j++ ){
			tmp += histogram.at(j);
		}
		distribution[i] = double(tmp) / double((img->width()*img->height()));
	}

	double d0 = 0.0;
	for( int i = 0; i < distribution.count(); i++ ){
		if( distribution.at(i) > 0 ){
			d0 = distribution.at(i);
			break;
		}
	}
	QVector<int> lut;
	lut.fill( 0,256 );
	for( int i = 0; i < 256; i++ ){
		lut[i] = floor( ((distribution.at(i) - d0)/ (1.0 - d0))*(256.0-1.0));
	}

	QVector<QRgb> colorTable = img->colorTable();
	for( int i = 0; i < colorTable.count(); i++ ){
		QRgb pixel = colorTable.at(i);
		pixel = qRgb( lut[qRed(pixel)], lut[qRed(pixel)],lut[qRed(pixel)]);
		colorTable.replace(i,pixel);
	}
	img->setColorTable( colorTable );
}
