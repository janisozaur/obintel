#include <QtCore/QCoreApplication>
#include <QDebug>

#include "facesextractor.h"
#include "cfgreader.h"
#include "floatfann.h"
#include "fann_cpp.h"

const QString cfgPath("cfg.txt");

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Configuration *cfg = CfgReader::readConfiguration(cfgPath);
    FacesExtractor *facesExtractor = new FacesExtractor(cfg->getDirectory());
    QList<QImage> imageList = facesExtractor->getFaces(cfg->getFaceSize());
    //wywołać sieć dla losowo wygranego elementu imageList
    //po wszystkim zapisać konfigurację sieci do pliku
    qDebug() << "Liczba twarzy: " << imageList.length();
    imageList.clear();
    delete cfg;
    delete facesExtractor;
    return 0;
}
