#ifndef UTILS_H
#define UTILS_H
#include "microarray.h"
#include "configuration.h"

#include "fann.h"
#include "fann_activation.h"
#include "fann_train.h"

class Utils
{
public:
    Utils();
    ~Utils();
    Configuration _cfg;

    //przygotowuje plik z danymi treningowymi, dane treningowe to kolumny macierzy po grupowaniu, jedna próbka wejściowa to geny baseline i after
    void prepareTrainData(MicroArray *microArray, QList<int> representatives, int networkOutput, bool baseline);

    void prepareDataForSVM(MicroArray *microArray, QList<int> representatives, int networkOutput, bool baseline);

    //uczy kilka sieci na rozlacznych zbiorach danych treningowych, dane treningowe musza sie znajdowac w katalogu wskazywanycm przez cfg
    void train();

    //testuje wszystkie nauczone sieci na zbiorze danych testowych
    void test();

private:

    static bool ifprint;

    bool checkResult(float result, float expected);

    //zapisuje postepy nauki sieci, wywolywana co okreslona ilosc epok
    static int printCallback(struct fann *net, struct fann_train_data *train,
            unsigned int max_epochs, unsigned int epochs_between_reports,
            float desired_error, unsigned int epochs);

};

#endif // UTILS_H
