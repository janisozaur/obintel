#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "facesextractor.h"
#include "cfgreader.h"
#include "floatfann.h"
#include "fann_cpp.h"

const QString cfgPath("cfg.txt");

// Callback function that simply prints the information to cout
int printCallback(FANN::neural_net &net, FANN::training_data &train,
	unsigned int max_epochs, unsigned int epochs_between_reports,
	float desired_error, unsigned int epochs, void *user_data)
{
	qDebug() << "epoka: " << epochs << " " << "blad: " << net.get_MSE();
	//zapisac do pliku
	return 0;
}

void train(Configuration *cfg)
{

	const float learning_rate = 0.9f;
	const unsigned int numLayers = 3;
	const unsigned int numInput = 1024;
	const unsigned int numHidden = 32;
	const unsigned int numOutput = 1;
	const float desiredError = 0.0001f;
	const unsigned int maxIterations = 1000;
	const unsigned int iterationsBetweenReports = 100;

	FANN::neural_net net;
	net.create_standard(numLayers, numInput, numHidden, numOutput);
	net.set_learning_rate(learning_rate);
	net.set_activation_steepness_hidden(0.5);
	net.set_activation_steepness_output(0.5);
	net.set_learning_momentum(0.6);
	net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC);
	net.set_activation_function_output(FANN::SIGMOID_SYMMETRIC);
	net.set_training_algorithm(FANN::TRAIN_RPROP);
	net.print_parameters();

	FANN::training_data data;
	if (data.read_train_from_file(cfg->getDataSavePath().toStdString()))
	{
		qDebug() << "Wczytano dane";
		//inicjalizacja wag
		net.init_weights(data);
		data.shuffle_train_data();
		net.set_callback(printCallback, NULL);
		net.train_on_data(data, maxIterations,
			iterationsBetweenReports, desiredError);
		net.save(cfg->getNetSavePath().toStdString());
		qDebug() << "Nauczono i zapisano siec";
	}
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
	Configuration *cfg = CfgReader::readConfiguration(cfgPath);
	QString param;
	if(argc > 1){
		param = argv[1];
	} else {
		param = "";
	}
	//przygotowanie danych
	qDebug() << param;
	if(argc == 1 || param == "-data" || param == "-data_box"){
		QList<QImage> facesList;
		QList<QImage> nonFacesList;
		if(param == "-data_box"){//dla twarzy bez coordynatów twarzy
			facesList = FacesExtractor::getFaces(cfg->getFaceSize(),
															   cfg->getFacesDirectory());
			nonFacesList = FacesExtractor::getFaces(cfg->getFaceSize(),
																  cfg->getNonFacesDirectory());
		}
		if(param == "-data"){//dla zdjęc z coordynatami twarzy
			facesList = FacesExtractor::getFacesWithoutCoords(cfg->getFaceSize(),
															   cfg->getFacesDirectory());
			nonFacesList = FacesExtractor::getFacesWithoutCoords(cfg->getFaceSize(),
																  cfg->getNonFacesDirectory());
		}


		FacesExtractor::saveFacesToFile(cfg->getDataSavePath(), facesList, nonFacesList,
										cfg->getFaceSize().width()*cfg->getFaceSize().height());

		qDebug() << "Znaleziona ilosc probek w zestawie treningowym: "
				 << "faces: " << facesList.length() << " nonfaces: " << nonFacesList.length();
		facesList.clear();
		nonFacesList.clear();
	}

	//uczenie sieci
	if(argc == 1 || param == "-train"){
		train(cfg);
	}
    return 0;
}
