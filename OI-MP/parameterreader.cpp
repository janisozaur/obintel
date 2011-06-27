#include "parameterreader.h"

#include <QDebug>
#include <QStringList>

#include "configuration.h"
#include "utils.h"
#include "pearson.h"
#include "euclidean.h"
#include "spearman.h"
#include "microarray.h"
#include "kmeans.h"

#include <QFile>
#include <QString>

ParameterReader::ParameterReader() : _input(stdin)
{
}

void ParameterReader::readParameters(){
    QString command;

    do{
        qDebug() << ">> ";
        command = _input.readLine();
        chooseAction(command);

    } while(!command.startsWith("exit"));
}

void ParameterReader::chooseAction(QString command){

    Configuration cfg;
    Utils utils;
    utils._cfg = cfg;

    QStringList parameters = command.split(QRegExp("\\s+"));

    if("data" == parameters.at(0)){

        qDebug() << "ROBI SIE!";

        QString microArrayFile = cfg._microArrayFile;

        MicroArray* microArray = new MicroArray();
        microArray->_numbersOfGroups = cfg._endGroup;
        microArray->_startGroup = cfg._startGroup;
        microArray->parseTxtFile(microArrayFile);

        qDebug() << "Liczba wierszy macierzy: " << microArray->_matrix.size();
        qDebug() << "Liczba kolumn macierzy: " << microArray->_matrix[0].size();
        qDebug() << "*******************" ;

        Kmeans kmeans;
        kmeans._k = cfg._k;
        kmeans._iterations = cfg._iterations;
        kmeans._distance = new Pearson();
        kmeans.run(microArray);

        QList<int> representatives = kmeans.picUpRepresentatives(microArray);

        utils.prepareTrainData(microArray, representatives, 1, true);
        //utils.prepareDataForSVM(microArray, representatives, 1, true);

        QString s = "D:/oi_mp/liczebnosc_grup.log";
        QFile f(s);
        f.open(QFile::WriteOnly | QFile::Text);
        QTextStream out(&f);
        for(int i = 0; i < kmeans._numberInCentr.size(); ++i){
            out << i << " " << kmeans._numberInCentr[i] << "\n";
        }
        f.close();

    } else if("train" == parameters.at(0)){

        utils.train();

    } else if("test" == parameters.at(0)){

        utils.test();

    } else if("exit" == parameters.at(0)){

        return;

    } else{

        qDebug() << "Niewlasciwy parametr: " << parameters.at(0);
    }
}
