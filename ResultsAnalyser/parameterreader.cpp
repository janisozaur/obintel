#include "parameterreader.h"
#include "configuration.h"
#include "utils.h"

#include <QDebug>
#include <QStringList>

ParameterReader::ParameterReader() : _input(stdin)
{
}

void ParameterReader::readParameters(){
    QString command;

    int cnt = 0;
    do{
        qDebug() << ">> ";
        if (_input.atEnd()) {
            command = "exit";
        } else {
            command = _input.readLine();
            if (command.isEmpty()) {
                if (++cnt == 3) {
                    command = "exit";
                }
            }
        }
        chooseAction(command);

    } while(!command.startsWith("exit"));
}

void ParameterReader::chooseAction(QString command){

    Configuration cfg;
    QStringList parameters = command.split(QRegExp("\\s+"));

    cfg._orginalDir = parameters.at(0);
    cfg._resultsDir = parameters.at(1);
    cfg._resultFile = parameters.at(2);
    cfg._reconThreshold = parameters.at(3).toDouble();

    Utils::analyseResults(cfg);

}
