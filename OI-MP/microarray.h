#ifndef MICROARRAY_H
#define MICROARRAY_H
#include <QList>
#include <QVector>
#include <QTextStream>

//Klasa przechowuje te dziwne liczby z tego pliku co byl na tej stronie
class MicroArray
{
public:
    MicroArray();
	~MicroArray();

	//wczytuje macierz z pliku
	void parseTxtFile(QString fileName);

	//macierz, której wiersze zawierają ciąg wartości danego genu dla wszystkich badanych osobników
	//czyli wiersz z pliku txt np.	8.79835615237719	8.31195146646897	...
    QList< QList<double> > _matrix;

	//etykiety osobników np. GMS339455
	QList< QString > _individualLabels;

	//etykiety genów np.	"10001_at"	"1007_at"	...
	QList< QString > _geneLabels;

	//liczba grup w naszym wypadku powinna być równa 4 bo PUFA(przed i po) i SUFA(przed i po)
	int _numbersOfGroups;

	//liczba elementów w grupie w naszym wypadku powinna być równa 21(policzyłem)
	int _numbersInGroups;

	//od której grupy zaczyna się wczytywanie
	//możemy wczytać wszytkie 4 albo np tylo PUFA baseline i PUFA after
	//grupy numerujemy od zera czyli aby wczytać wszystkie cztery podajemy
	//_startGroup = 0; numbersOfGroup = 4;
	int _startGroup;

private:
	//rozpoczyna wczytywanie zawartosci pliku
	void readMatrix(QTextStream& input);
	//wczytuje etykiety osobników
	void readIndividualLabels(QTextStream& input);
	//wczytuje wartości i etykiety genów
	void readGeneValues(QTextStream& input);

};

#endif // MICROARRAY_H
