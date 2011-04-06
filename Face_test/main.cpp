#include <QtCore/QCoreApplication>
#include "cfgreader.h"
#include <QRect>
#include <QList>
#include <QDebug>
#include "imagescanner.h"

const QString cfgPath("cfg.txt");

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    Configuration *cfg = CfgReader::readConfiguration(cfgPath);
	ImageScanner imageScanner(cfg);
	imageScanner.scannDir();

	return 0;
}
