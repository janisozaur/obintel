#ifndef UTILS_H
#define UTILS_H
#include <QFile>
#include <QTextStream>
#include <QImage>

#include "configuration.h"

//#include "floatfann.h"
//#include "fann_cpp.h"

#include "fann.h"
#include "fann_activation.h"
#include "fann_train.h"


class Utils
{
public:

    static void trainNetwork(Configuration& cfg);

    static void testNetwork(Configuration& cfg);

	static void prepareData(Configuration& cfg);

private:

	Utils();

    //zapisuje postepy nauki sieci, wywolywana co okreslona ilosc epok
	static int printCallback(struct fann *net, struct fann_train_data *train,
			unsigned int max_epochs, unsigned int epochs_between_reports,
			float desired_error, unsigned int epochs);

	static QList<QRect> scaleImage(const QImage &image, Configuration &cfg, struct fann *ann, struct fann *ann2);

	static QList<QRect> scannImage(const QImage &image, Configuration &cfg, struct fann *ann, struct fann *ann2);

	static bool checkRect(const QImage &face, Configuration &cfg,  struct fann *ann, struct fann *ann2);

	static QList<QImage> prepareFaces(Configuration& cfg);

	static QList<QImage> prepareNonFaces(Configuration& cfg);

	static QList<QRect> getFacesCoords(QString coordsFile);

	static void saveTrainData(Configuration& cfg, QList<QImage> faces, QList<QImage> nonFaces);

	static void histogramEqualization(QImage *img);

};

#endif // UTILS_H
