#include "utils.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>
#include <QDateTime>
#include <QStringList>
#include <cmath>

Utils::Utils()
{
}

Utils::~Utils(){
}

bool Utils::ifprint = true;

void Utils::prepareTrainData(MicroArray *microArray, QList<int> representatives, int networkOutput, bool baseline){

    QFile file(_cfg._trianDataFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Nie mozna otworzyc pliku do zapisu danych treningowych";
        return;
    }

    QTextStream out(&file);

    out << _cfg._numbersInGroups << " " << representatives.size() * 2 << " " << 1 << endl; //naglowek pliku treningowego

    QList<double> values;

    int temp = 1;//(microArray->_numbersOfGroups - microArray->_startGroup)/2;

    for(int i = 0; i < temp * microArray->_numbersInGroups; ++i){

        if(baseline){

            for(int j = 0; j < representatives.size(); ++j){
                values = microArray->_matrix[representatives[j]];

                out << values[i] << " ";
            }
        }

        for(int j = 0; j < representatives.size(); ++j){
            values = microArray->_matrix[representatives[j]];

            out << values[i + microArray->_numbersInGroups] << " ";
        }

        out << endl;
        out << networkOutput << endl;
    }

    file.close();
}

void Utils::prepareDataForSVM(MicroArray *microArray, QList<int> representatives, int networkOutput, bool baseline){
    QFile file("D:/dataForSVM.dat");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Nie mozna otworzyc pliku do zapisu danych treningowych";
        return;
    }

    QTextStream out(&file);

    QList<double> values;

    int temp = 1;//(microArray->_numbersOfGroups - microArray->_startGroup)/2;

    for(int i = 0; i < temp * microArray->_numbersInGroups; ++i){

        out << networkOutput << " ";

        if(baseline){

            for(int j = 0; j < representatives.size(); ++j){
                values = microArray->_matrix[representatives[j]];

                out << values[i] << " ";
            }
        }

        for(int j = 0; j < representatives.size(); ++j){
            values = microArray->_matrix[representatives[j]];

            out << values[i + microArray->_numbersInGroups] << " ";
        }

        out << endl;
    }

    file.close();
}

void Utils::train(){

    //dane treningowe
    QDir dir(_cfg._trainDataDir);
    dir.setFilter(QDir::Files);
    QStringList filters;
    filters << "*.dat";
    dir.setNameFilters(filters);
    dir.setSorting(QDir::Name);
    QFileInfoList fileList = dir.entryInfoList();

    //dane validacyjne
    QDir valDir(_cfg._validDataDir);
    valDir.setFilter(QDir::Files);
    QStringList filters2;
    filters2 << "*.dat";
    valDir.setNameFilters(filters2);
    valDir.setSorting(QDir::Name);
    QFileInfoList validFileList = valDir.entryInfoList();

    qDebug() << "Liczba zbiorow treningowych: " <<  fileList.size();
    qDebug() << "Liczba zbiorow walidacyjnych: " << validFileList.size();
    fann *ann = NULL;

    fann_train_data *valData = NULL;

    float avgMSE = 0;

    for(int i = 0; i < fileList.size(); ++i){

        //tworzenie sieci
        ann = fann_create_standard(_cfg._numLayers, _cfg._numInput, _cfg._numHidden, _cfg._numOutput);

        valData = fann_read_train_from_file(validFileList.at(i).absoluteFilePath().toStdString().c_str());

        //ustawianie parametrów sieci
        fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
        fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
        fann_set_activation_steepness_hidden(ann, 0.5);
        fann_set_activation_steepness_output(ann, 0.5);
        fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);
        fann_set_learning_rate(ann, _cfg._learningRate);
        fann_set_learning_momentum(ann, _cfg._momentum);
        fann_set_callback(ann, printCallback);

        //przygotowanie pliku logow
        QFile logFile("mse.log");
        if (!logFile.open(QIODevice::Append | QIODevice::Text)){
            qWarning() << "Nie mozna otworzyc pliku do logowania";
        }
        QTextStream logStream(&logFile);
        logStream << "ANN: " << i << "\t" <<  QDateTime::currentDateTime().toString() << endl;
        logFile.close();

        //nauka
        fann_train_on_file(ann, fileList.at(i).absoluteFilePath().toStdString().c_str(), _cfg._epochs, _cfg._epochsBetweenReports, _cfg._desiredError);

        //walidacja
        avgMSE += fann_test_data(ann, valData);

        //zapisanie nauczonej sieci
        QString s((_cfg._annSaveDir + "ann-%1.ann").arg(i));
        fann_save(ann, s.toStdString().c_str());

        delete ann;
        ann = NULL;

        delete[] valData;
        valData = NULL;

        qDebug() << "Nauczono i zapisano siec: " + s;
    }

    qDebug() << "Sredni blad klasyfikatora: "  << avgMSE/fileList.size();

}

void Utils::test(){

    QDir dir(_cfg._annSaveDir);
    dir.setFilter(QDir::Files);
    QStringList filters;
    filters << "*.ann";
    dir.setNameFilters(filters);
    dir.setSorting(QDir::Name);
    QFileInfoList fileList = dir.entryInfoList();

    QList<fann*> annList;
    for(int i = 0; i < fileList.size(); ++i){
        annList.append(fann_create_from_file(fileList.at(i).absoluteFilePath().toStdString().c_str()));
    }

    QFile file(_cfg._trianDataFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Nie mozna otworzyc pliku z danymi testowymi";
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine(); //wczytany naglowek pliku, teraz niepotrzebny
    QStringList splitLine;
    float *input = NULL;
    float output = 0;
    float result = 0;
    int numSamples = 0;
    int numCorrect = 0;

    while(!in.atEnd()){
        line = in.readLine();
        splitLine = line.split(" ");
        input = new float[splitLine.size()];

        for(int i = 0; i < splitLine.size(); ++i){

            input[i] = splitLine.at(i).toFloat();
        }

        line = in.readLine();
        output = line.toFloat();

        result = 0;
        //testowanie dla wszystkich sieci
        for(int j = 0; j < annList.size(); ++j){

            result += fann_run(annList.at(j), input)[0];

        }

        qDebug() << result/annList.size() << "\t" << output;

        if(checkResult(result/annList.size(), output)){
            ++numCorrect;
        }

        ++numSamples;

        delete[] input;
        input = NULL;

    }

    file.close();

    qDebug() << "Prawidlowo wykryto: " << 100 * numCorrect/numSamples << "%";

}

bool Utils::checkResult(float result, float expected){
    if(std::abs(expected) - std::abs(result) < _cfg._testThreshold){
        return true;
    }
    return false;
}

int Utils::printCallback(struct fann *net, struct fann_train_data *train,
                         unsigned int max_epochs, unsigned int epochs_between_reports,
                         float desired_error, unsigned int epochs)
{
    if(ifprint){
        //zapisac do pliku
        QFile file("mse.log");
        if (!file.open(QIODevice::Append | QIODevice::Text)){
            qWarning() << "Nie mozna otworzyc pliku do logowania";
        }
        QTextStream stream(&file);
        qDebug() << "MSE: " << fann_get_MSE(net);
        stream << fann_get_MSE(net) << endl;
        file.close();
        ifprint = false;

    } else{
        ifprint = true;
    }
    return 0;
}
