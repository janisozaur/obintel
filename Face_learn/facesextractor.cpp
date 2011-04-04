#include "facesextractor.h"
#include <QDebug>
#include <QPoint>

FacesExtractor::FacesExtractor(QDir dir) {
    _dir = dir;
}

QList<QImage> FacesExtractor::getFaces(QSize size) {
    QList<QImage> imageList;
    _dir.setFilter(QDir::Files);
    QFileInfoList fileList = _dir.entryInfoList();

    for(int i = 0; i < fileList.length(); i += 2){
        qDebug() << fileList.at(i).absoluteFilePath();
        QImage image(fileList.at(i).absoluteFilePath());
        if(image.isNull()){
            qWarning() << "Nie można otworzyć zdjęcia: " + fileList.at(i).absoluteFilePath();
            continue;
        }
        QFile coordFile(fileList.at(i+1).absoluteFilePath());
        QList<QRect> facesCoords = getFacesCoords(coordFile);

        foreach(QRect rect , facesCoords){
            QImage face = image.copy(rect);
            if(!face.isNull()){
                imageList.append(face.scaled(size));
            }
        }
    }
    return imageList;
}

QList<QRect> FacesExtractor::getFacesCoords(QFile &coordFile) {
    QList<QRect> coords;
    if (!coordFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "Nie można otworzyć pliku z konfiguracją: " + coordFile.fileName();
        return coords;
    }
    QTextStream coordStream(&coordFile);
    while(!coordStream.atEnd()){
        QString faceCoord = coordStream.readLine();
        QStringList coordList = faceCoord.split(" ");
        QPoint topLeft(coordList.at(0).toInt(), coordList.at(1).toInt());
        QPoint bottomRight(coordList.at(2).toInt(), coordList.at(3).toInt());
        QRect faceRect(topLeft, bottomRight);
        coords.append(faceRect);
    }
    coordFile.close();
    return coords;
}
