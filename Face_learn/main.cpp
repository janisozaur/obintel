#include "facesextractor.h"
#include "cfgreader.h"

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <floatfann.h>
#include <fann_cpp.h>
#include <QSettings>

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
	QString fileName(QDir::homePath() + "/" + QCoreApplication::applicationName() + ".ini");
	qDebug() << "using config file:" << fileName;
	QSettings settings(fileName, QSettings::IniFormat);
	const float learningRate = settings.value("learningRate", 0.8).toFloat();
	const unsigned int numLayers = settings.value("numLayers", 3).toInt();
	const unsigned int numInput = settings.value("numInput", 1024).toInt();
	const unsigned int numHidden = settings.value("numHidden", 32).toInt();
	const unsigned int numOutput = settings.value("numOutput", 1).toInt();
	const float desiredError = settings.value("desiredError", 0.0001f).toFloat();
	const unsigned int maxIterations = settings.value("maxIterations", 3000).toInt();
	const unsigned int iterationsBetweenReports = settings.value("iterationsBetweenReports", 100).toInt();

	FANN::neural_net net;
	net.create_standard(numLayers, numInput, numHidden, numOutput);
	net.set_learning_rate(learningRate);
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
	QCoreApplication::setApplicationName("faces");
	QCoreApplication::setOrganizationDomain("wacjan.com");
	QCoreApplication::setOrganizationName("wacjan");
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
