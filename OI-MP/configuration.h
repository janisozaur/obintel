#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QString>

class Configuration
{
public:
    Configuration();

    int _iterations;
    int _k;
    int _numbersInGroups;
    int _endGroup;
    int _startGroup;
    QString _microArrayFile;

    int _epochsBetweenReports;
    int _desiredError;
    int _momentum;
    int _learningRate;
    int _numHidden;
    int _numInput;
    int _numOutput;
    int _numLayers;
    int _maxEpochs;
    int _epochs;

    float _testThreshold;


    QString _trianDataFile;
    QString _validDataDir;
    QString _trainDataDir;
    QString _annSaveDir;
};

#endif // CONFIGURATION_H
