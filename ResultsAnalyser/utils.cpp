#include "utils.h"
#include <QDir>
#include <QDebug>
#include <QDateTime>

Utils::Utils()
{
}

void Utils::analyseResults(Configuration &cfg){
    QDir orginalDir(cfg._orginalDir);
    QDir resultsDir(cfg._resultsDir);

    QStringList filters;
    filters << "*.txt";
    orginalDir.setNameFilters(filters);
    resultsDir.setNameFilters(filters);

    //zakladam ze nazwy plikow wynikow beda odpowiadac plikom orginalnym
    QFileInfoList orginalFileList = orginalDir.entryInfoList(QDir::Files, QDir::Name);
    QFileInfoList resultsFileList = resultsDir.entryInfoList(QDir::Files, QDir::Name);

    if(orginalFileList.size() != resultsFileList.size()){
        qWarning() << "Liczba plikow wynikow jest inna niz liczba plikow oryginalnych - ZUO";
        return;
    }

    QList<QRect> orginalCoords;
    QList<QRect> resultsCoords;
    QList< QPair<double, double> > result;

    for(int i = 0; i < orginalFileList.size(); ++i){

        orginalCoords = getCoords(orginalFileList.at(i).absoluteFilePath());
        resultsCoords = getCoords(resultsFileList.at(i).absoluteFilePath());

        result.append(compareCoords(orginalCoords, resultsCoords, cfg._reconThreshold));

    }

    saveResults(result, cfg._resultFile);
}

void Utils::saveResults(QList<QPair<double, double> > result, QString resultFile){
    QFile file(resultFile);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qWarning() << "Nie mozna otworzyc pliku do zapisu wynikow";
        return;
    }

    double avgFace = 0.0;
    double avgFalse = 0.0;
    double min = 100.0;
    double max = 0.0;

    for(int i = 0; i < result.size(); ++i){

        avgFace += result.at(i).first;
        avgFalse += result.at(i).second;

        if(result.at(i).first > max){
            max = result.at(i).first;
        }
        if(result.at(i).first < min){
            min = result.at(i).first;
        }
    }

    QTextStream stream(&file);

    stream << QDateTime::currentDateTime().toString() << endl;
    stream << "Average face detection:\t" << avgFace/result.size() << "\%" << endl;
    stream << "Average false detection:\t" << avgFalse/result.size() << endl;
    stream << "Minimum face detection:\t" << min << "\%" << endl;
    stream << "Maximum face detection:\t" << max << "\%" << endl;
    stream << "** DATA PER IMAGE **" << endl;

    for(int i = 0; i < result.size(); ++i){
        stream << i << "\t" << result.at(i).first << "\t" << result.at(i).second << endl;
    }

    file.close();
}

QPair<double, double> Utils::compareCoords(const QList<QRect> &orginalCoords, const QList<QRect> &resultsCoords, double threshold){

    int numPositive = 0;//liczba pozytywnych trafien, nawet zduplikowanych
    int numFaces = 0;//liczba trafien z pominieciem duplikatow

    for(int i = 0; i < orginalCoords.size(); ++i){

        int duplicates = 0;

        for(int j = 0; j < resultsCoords.size(); ++j){

            if(compareRects(orginalCoords.at(i), resultsCoords.at(j), threshold)){
                ++numPositive;
                ++duplicates;
            }
        }

        //pominiecie duplikatow
        if(duplicates > 0){
            ++numFaces;
        }
    }

    QPair<double, double> result;
    result.first = ((double)numFaces/orginalCoords.size())*100.0;
    result.second = resultsCoords.size() - numPositive;

    return result;
}

bool Utils::compareRects(const QRect &orginalRect, const QRect &resultRect, double threshold){

    QRect intersected = orginalRect.intersected(resultRect);
    double interArea = calcArea(intersected);
    double orginalArea = calcArea(orginalRect);
    double resultArea = calcArea(resultRect);

    //trzeba pamietac ze znaleziony kwadrat moze zawierac w sobie orginalny ale byc o wiele wiekszy z tego powodu drugi warunek
    if((interArea / orginalArea) > threshold && (orginalArea / resultArea) > threshold){
        return true;
    }

    return false;
}

double Utils::calcArea(const QRect &rect){
    return (rect.width() * rect.height());
}

QList<QRect> Utils::getCoords(QString fileName){
    QFile file(fileName);
    QList<QRect> coords;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "Nie mozna odczytac pliku z koordynatami";
        return coords;
    }

    QTextStream stream(&file);

    while(!stream.atEnd()){

        QString faceCoord = stream.readLine();
        QStringList coordList = faceCoord.split(" ");
        QPoint topLeft(coordList.at(0).toInt(), coordList.at(1).toInt());
        QPoint bottomRight(coordList.at(2).toInt(), coordList.at(3).toInt());
        QRect faceRect(topLeft, bottomRight);
        coords.append(faceRect);
    }

    file.close();
    return coords;
}
