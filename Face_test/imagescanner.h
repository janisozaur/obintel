#ifndef IMAGESCANNER_H
#define IMAGESCANNER_H
#include <QImage>
#include "configuration.h"
#include "floatfann.h"
#include "fann_cpp.h"

class ImageScanner
{
public:
    ImageScanner(Configuration *cfg);
    void scannDir();

private:
    Configuration *_cfg;
    const QSize _minImageSize;
    const QSize _imageScaleStep;
    const QPoint _startFaceBoxPoint;
    const QPoint _faceBoxPointStep;
	FANN::neural_net _net;
	FANN::neural_net _net2;

	QList<QRect> scannPicture(const QImage &scaledImage, const QImage &orginalImage);
    bool checkRect(const QImage &face);
    QList<QRect> scaleImage(const QImage &image);
	void histogramEqualization(QImage *img);


};

#endif // IMAGESCANNER_H
