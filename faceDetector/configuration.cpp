#include "configuration.h"

Configuration::Configuration()
{
	//nauka
    this->_desiredError = 0.001;
	this->_epochsBetweenReports = 25;
	this->_epochs = 50;
	this->_totalEpochs = 1000;
    this->_learningRate = 0.7;
    this->_momentum = 0.9;
	this->_numInput = 1024;
    this->_numHidden = 32;
    this->_numOutput = 1;
    this->_annSaveFile = "ann.txt";
    this->_dataFile = "data.txt";
    this->_valDataFile = "valData.txt";
	this->_valThreshold = 0.001;

	//testowanie
	this->_annSaveFile2 = this->_annSaveFile;
	this->_directory = "D:/faceTest";
	this->_faceSize.setHeight(32);
	this->_faceSize.setWidth(32);
	this->_faceBoxPointStep.setX(8);
	this->_faceBoxPointStep.setY(4);
	this->_startFaceBoxPoint.setX(0);
	this->_startFaceBoxPoint.setY(0);
	this->_minImageSize.setHeight(24);
	this->_minImageSize.setWidth(32);
	this->_imageScaleStep.setHeight(24);
	this->_imageScaleStep.setWidth(32);
	this->_orginalImageSize.setHeight(768);
	this->_orginalImageSize.setWidth(1024);
	this->_testThreshold = 0.99;

	this->networks = NULL;
	this->networks2 = NULL;
	this->faceData = NULL;

	this->threads = 1;
}
