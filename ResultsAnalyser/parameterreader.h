#ifndef PARAMETERREADER_H
#define PARAMETERREADER_H
#include <QString>
#include <QTextStream>

class ParameterReader
{
public:
    ParameterReader();
    void readParameters();
private:
    void chooseAction(QString command);
    QTextStream _input;
};

#endif // PARAMETERREADER_H
