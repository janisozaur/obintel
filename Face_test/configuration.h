#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QSize>
#include <QDir>

class Configuration
{
public:
    Configuration();
    Configuration(QDir directory, QSize faceSize);
    Configuration(Configuration &cfg);

    void setFaceSize(QSize faceSize);
    void setDirectory(QDir directory);

    QSize getFaceSize();
    QDir getDirectory();

private:
    QDir _directory;
    QSize _faceSize;
};

#endif // CONFIGURATION_H
