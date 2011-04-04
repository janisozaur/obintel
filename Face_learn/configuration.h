#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QSize>
#include <QDir>

class Configuration
{
public:
    Configuration();
    Configuration(QDir directory, QSize faceSize);

    void setFaceSize(QSize faceSize);
    void setDirectory(QDir directory);

    QSize getFaceSize() const;
    QDir getDirectory() const;

private:
    QDir _directory;
    QSize _faceSize;
};

#endif // CONFIGURATION_H
