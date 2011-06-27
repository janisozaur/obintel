#include "configuration.h"

Configuration::Configuration()
{

    //data
    this->_iterations = 10;
    this->_k = 20;
    this->_numbersInGroups = 21;
    this->_endGroup = 2;
    this->_startGroup = 0;
    this->_microArrayFile = "D:/oi_mp/microarray.txt";

    //train
    this->_numLayers = 3;
    this->_momentum = 0.0;
    this->_learningRate = 0.7;
    this->_numHidden = 9;
    this->_numInput = 20;
    this->_numOutput = 1;
    this->_maxEpochs = 200;
    this->_epochs = 25;
    this->_epochsBetweenReports = 25;
    this->_desiredError = 0.001;


    this->_testThreshold = 0.2;

    this->_trainDataDir = "D:/oi_mp/trainDataDir";
    this->_trianDataFile = "D:/oi_mp/trainDataFile.dat";
    this->_validDataDir = "D:/oi_mp/validDataDir";
    this->_annSaveDir = "D:/oi_mp/annSaveDir";
}
