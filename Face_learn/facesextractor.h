#ifndef IMAGESCANNER_H
#define IMAGESCANNER_H

#include <QDir>
#include <QRect>
#include <QList>
#include <QImage>
#include <QTextStream>

class FacesExtractor {

public:
	static QList<QImage> getFaces(QSize size, QDir dir);
	static QList<QImage> getFacesWithoutCoords(QSize size, QDir dir);
	static void saveFacesToFile(QString filePath, const QList<QImage> &facesList,
								const QList<QImage> &nonFacesList, int inputs);


private:
		FacesExtractor();
	static QList<QRect> getFacesCoords(QFile &textStream);
	static void histogramEqualization(QImage *img);

};

#endif // IMAGESCANNER_H
