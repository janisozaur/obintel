#ifndef PARAMETERREADER_H
#define PARAMETERREADER_H

#include <QTextStream>

class ParameterReader
{
public:
    ParameterReader();
    void readParameters();
private:
    void chooseAction(QString parameters);
    QTextStream _input;
};

#endif // PARAMETERREADER_H
