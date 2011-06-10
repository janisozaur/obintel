#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QSize>
#include <QDir>

class Configuration
{
public:
	Configuration(QDir directory, QSize faceSize, QString netPath, QString netPath2);
    Configuration(Configuration &cfg);

    QSize getFaceSize();
    QDir getDirectory();
	QString getNetPath();
	QString getNetPath2();

private:
	Configuration();
    QDir _directory;
    QSize _faceSize;
	QString _netPath;
	QString _netPath2;
};

#endif // CONFIGURATION_H
