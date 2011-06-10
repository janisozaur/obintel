#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QSize>
#include <QDir>
#include <QString>

class Configuration
{
public:

	Configuration(QDir facesDirectory, QDir nonFacesDirectory, QSize faceSize,
				  QString dataSavePath, QString netSavePath);
    QSize getFaceSize() const;
	QDir getFacesDirectory() const;
	QDir getNonFacesDirectory() const;
	QString getDataSavePath() const;
	QString getNetSavePath() const;

private:
	Configuration();
	QDir _facesDirectory;
	QDir _nonFacesDirectory;
    QSize _faceSize;
	QString _dataSavePath;
	QString _netSavePath;
};

#endif // CONFIGURATION_H
