#ifndef IMAGESCANNER_H
#define IMAGESCANNER_H
#include <QImage>
#include "configuration.h"

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

    QList<QRect> scannPicture(const QImage &image);
    bool checkRect(const QImage &face);
    QList<QRect> scaleImage(const QImage &image);


};

#endif // IMAGESCANNER_H
