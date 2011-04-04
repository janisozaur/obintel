#include <QtCore/QCoreApplication>
#include "cfgreader.h"
#include <QRect>
#include <QList>
#include <QDebug>
#include <QRgb>
#include "imagescanner.h"


const QString cfgPath("cfg.txt");

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Configuration *cfg = CfgReader::readConfiguration(cfgPath);
	ImageScanner ImageScanner(cfg);
	QImage image("D:/image.png");
	image = image.convertToFormat(QImage::Format_RGB32);
	QRgb *tab = (QRgb*)image.bits();
	qDebug() << qRed(*tab);
	qDebug() << qBlue(*tab);
	qDebug() << qGreen(*tab);

	//    ImageScanner.scannPicture(image);
	//    QSize s1(10,10);
	//    QSize s2(30,30);
	//    s2 -= s1;
	//    qDebug() << s2.width();
    return a.exec();
}
