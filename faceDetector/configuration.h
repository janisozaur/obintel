#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QSize>
#include <QPoint>

struct fann;

class Configuration
{
public:
    Configuration();

    //nauka
    int _numInput;
    int _numHidden;
    int _numOutput;
    float _learningRate;
    float _momentum;
    int _epochsBetweenReports;
    int _desiredError;
    int _epochs;
	int _totalEpochs;
    QString _dataFile;
    QString _annSaveFile;
    QString _valDataFile;
	float _valThreshold;

    //testowanie
	QString _annSaveFile2;
	QSize _minImageSize;
	QSize _imageScaleStep;
	QPoint _startFaceBoxPoint;
	QPoint _faceBoxPointStep;
	QSize _faceSize;
	QString _directory;
	QSize _orginalImageSize;
	float _testThreshold;

	struct fann **networks;
	struct fann **networks2;

	int threads;
};

#endif // CONFIGURATION_H
