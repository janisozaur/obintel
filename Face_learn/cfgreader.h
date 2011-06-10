#ifndef CONFIGREADER_H
#define CONFIGREADER_H
#include <QDir>
#include <QSize>
#include "configuration.h"

class CfgReader
{
public:
    static Configuration* readConfiguration(QString cfgPath);

private:
    CfgReader();
};

#endif // CONFIGREADER_H
