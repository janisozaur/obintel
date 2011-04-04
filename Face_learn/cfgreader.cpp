#include "cfgreader.h"
#include <QDebug>
#include <QTextStream>

Configuration* CfgReader::readConfiguration(QString cfgPath){

    QFile file(cfgPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
             qWarning() << "Nie można otworzyć pliku z konfiguracją";
    }
    QTextStream stream(&file);
    QDir dir(stream.readLine());
    QStringList strList = stream.readLine().split(" ");
    QSize size(strList.at(0).toInt(), strList.at(1).toInt());
    file.close();

    return new Configuration(dir, size);
}

CfgReader::CfgReader()
{
}
