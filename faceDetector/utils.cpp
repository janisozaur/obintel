#include "utils.h"

#include <QDebug>
#include <QFileInfoList>
#include <QDir>
#include <QImage>
#include <cstring>
#include <QElapsedTimer>
#include <omp.h>

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


	QDir dir(cfg._directory);
	QStringList filters;
	filters << "*.png" << "*.jpg";
	dir.setNameFilters(filters);
	QFileInfoList fileList = dir.entryInfoList(QDir::Files);

	// use parallel-single construct in order to let omp_get_num_threads() get
	// correct thread count
#pragma omp parallel
	{
#pragma omp single
		{
			cfg.threads = omp_get_num_threads();
		}
	}
	// since fann is not thread-safe, but re-entrant, create as many networks
	// as many there are threads
	qDebug() << "threads:" << cfg.threads;
	cfg.networks = new struct fann *[cfg.threads];
	cfg.networks2 = new struct fann *[cfg.threads];
	for (int i = 0; i < cfg.threads; i++) {
		cfg.networks[i] = fann_create_from_file(cfg._annSaveFile.toStdString().c_str());
		cfg.networks2[i] = fann_create_from_file(cfg._annSaveFile2.toStdString().c_str());
	}

	QString total(QString::number(fileList.size()));
	QDir out = QDir::currentPath();
	bool created = out.mkpath(cfg._outputDirectory);
	if (!created) {
		qFatal("Cannot write results to %s", cfg._outputDirectory.toStdString().c_str());
	}
	out.cd(cfg._outputDirectory);
	QElapsedTimer t;
	t.start();
	for (int i = 0; i < fileList.size(); i++) {
		const QFileInfo fileInfo = fileList.at(i);

		QImage image(fileInfo.absoluteFilePath());
		image = image.convertToFormat(QImage::Format_RGB32);

		QList<QRect> faceBoxes(scaleImage(image, cfg));

		QFile file(out.path() + "/" + fileInfo.baseName() + ".txt");

		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
			qWarning() << "Nie mozna zapisac wynikow do " << file.fileName();
		}

		QTextStream stream(&file);
		for (int j = 0; j < faceBoxes.size(); j++) {
			const QRect rect = faceBoxes.at(j);
			stream << rect.topLeft().x() << " " << rect.topLeft().y() << " "
				   << rect.bottomRight().x() << " " << rect.bottomRight().y() << "\n";
		}
		file.close();

		qreal percent = (qreal)(i + 1) * 100.0 / fileList.size();
		QString percentStr(QString::number(percent));
		percentStr = percentStr.rightJustified(5, QLatin1Char(' '), true);
		QString info("%1\% (%2 of %3): %4");
		info = info.arg(percentStr,
						QString::number(i + 1).rightJustified(total.length()),
						total, fileInfo.filePath());
		qDebug() << info.toStdString().c_str();
	}
	for (int i = 0; i < cfg.threads; i++) {
		fann_destroy(cfg.networks[i]);
		fann_destroy(cfg.networks2[i]);
	}
	int msecs = t.elapsed();
	qDebug() << "time taken to process" << fileList.size() << "images:" << msecs
			 << "msecs. one picture per"
			 << (qreal)msecs / (qreal)fileList.size() << "msecs";
	delete [] cfg.networks;
	delete [] cfg.networks2;
	cfg.networks = NULL;
	cfg.networks2 = NULL;
}

QList<QRect> Utils::scaleImage(const QImage &image, Configuration &cfg)
{
	// don't force useless synchronization by not using the same shared list
	// in each of threads, but rather one-per-thread approach; concatenate them
	// into result after all work is done
	QList<QRect> *faces = new QList<QRect>[cfg.threads];

	int start = cfg._orginalImageSize.width();
	int end = cfg._minImageSize.width();
	int count = (start - end) / cfg._imageScaleStep.width();
	//qDebug() << "count:" << count;
	// since the time required to process each scaled image is variable, use
	// dynamic scheduling to achieve better workload balancing. the overhead
	// caused by dynamic scheduler is neglected by the program speedup
#pragma omp parallel for num_threads(cfg.threads) schedule(dynamic)
	for (int i = 0; i < count; i++)
	{
		QSize size = cfg._orginalImageSize - (i * cfg._imageScaleStep);
		const int thread = omp_get_thread_num();
		//qDebug() << "starting size:" << size << "thread:" << thread;
		//scannImage(image.scaled(size), cfg, ann);
		faces[thread].append(scannImage(image.scaled(size), cfg, thread));
		//qDebug() << "finished size:" << size << "thread:" << thread;
	}
	QList<QRect> result;
	for (int i = 0; i < cfg.threads; i++) {
		result.append(faces[i]);
	}
	delete [] faces;
	return result;
}

QList<QRect> Utils::scannImage(const QImage &image, Configuration &cfg, const int &thread)
{
	QList<QRect> faces;

	// fann is re-entrant, so don't waste resources on creating networks for
	// each thread, each image, each scale, but re-use them: one per thread
	struct fann *ann = cfg.networks[thread];
	struct fann *ann2 = cfg.networks2[thread];

	int start = cfg._startFaceBoxPoint.x();
	int end = image.width() - cfg._faceSize.width();
	int count = (end - start) / cfg._faceBoxPointStep.x();
	for (int i = 0; i < count; i++) {
		int startx = cfg._startFaceBoxPoint.x() + i * cfg._faceBoxPointStep.x();
		QPoint faceBoxMovePoint(startx, 0);
		QRect faceBox(cfg._startFaceBoxPoint, cfg._faceSize);
		faceBox.moveTo(faceBoxMovePoint);

		for(;faceBoxMovePoint.y() < image.height() - cfg._faceSize.height();
			faceBoxMovePoint.setY(faceBoxMovePoint.y() + cfg._faceBoxPointStep.y()))
		{
			faceBox.moveTo(faceBoxMovePoint);

			//jesli twarz to dodajemy do listy
			if(checkRect(image.copy(faceBox), cfg, ann, ann2)) {
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

bool Utils::checkRect(const QImage &face, Configuration &cfg, struct fann *ann, struct fann *ann2)
{
	//histogramEqualization(&img);

	const QRgb *tab = (QRgb*)face.constBits();
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
