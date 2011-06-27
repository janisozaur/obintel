#include "configuration.h"

Configuration::Configuration()
{

    //data
    this->_iterations = 100;
    this->_k = 600;
    this->_numbersInGroups = 21;
    this->_endGroup = 2;
    this->_startGroup = 0;
    this->_microArrayFile = "D:/oi_mp/microarray.txt";

    //train
    this->_numLayers = 3;
    this->_momentum = 0.6;
    this->_learningRate = 0.6;
    this->_numHidden = 35;
    this->_numInput = 1200;
    this->_numOutput = 1;
    this->_maxEpochs = 1000;
    this->_epochs = 200;
    this->_epochsBetweenReports = 25;
    this->_desiredError = 0.001;


    this->_testThreshold = 0.01;

    this->_trainDataDir = "D:/oi_mp/trainDataDir";
    this->_trianDataFile = "D:/oi_mp/trainDataFile.dat";
    this->_validDataDir = "D:/oi_mp/validDataDir";
    this->_annSaveDir = "D:/oi_mp/annSaveDir";
}
