#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QString>

class Configuration
{
public:
    Configuration();
    QString _orginalDir;
    QString _resultsDir;//katalog z plikami jakie wyplul FaceDetector
    QString _resultFile;
    double _reconThreshold;
};

#endif // CONFIGURATION_H
