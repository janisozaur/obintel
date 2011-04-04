#ifndef IMAGESCANNER_H
#define IMAGESCANNER_H

#include <QDir>
#include <QRect>
#include <QImage>
#include <QTextStream>

class FacesExtractor {

public:
    FacesExtractor(QDir dir);

    QList<QImage> getFaces(QSize size);

private:
    QDir _dir;

    QList<QRect> getFacesCoords(QFile &textStream);

};

#endif // IMAGESCANNER_H
