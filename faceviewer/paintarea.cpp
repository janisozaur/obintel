#include "paintarea.h"

#include <QPainter>
#include <QFile>
#include <QDir>
#include <QDebug>

/// @cond
PaintArea::PaintArea(QWidget *parent) :
	QWidget(parent),
	m_pImage(NULL)
{
}

PaintArea::~PaintArea(){
	delete m_pImage;
}
/// @endcond

/*! Zdarzenie malowania

  Wykorzystujemy go by namalować przy każdym zdarzeniu obrazek.

  @param event Wskaźnik do obiektu zdarzenia

 */
void PaintArea::paintEvent(QPaintEvent *e){
	if( !m_pImage ){
		QWidget::paintEvent(e);
		return;
	}

	QPainter painter(this);

	painter.drawImage( 0, 0, *m_pImage );
	painter.setPen( Qt::red );
	painter.drawRects( m_rects );
}

/*! Załadowanie obrazka

	Wczytujemy obrazek do pamięci, uaktualniamy geometrię widgetu, nakazujemy
	odrysować widget

  @param fileInfo Klasa z opisem pliku, ścieżka dostępu, etc...
  @return Stan powodzenia operacji, true gdy udało się wczytać obrazek, false
		  w przeciwnej sytuacji

 */
bool PaintArea::load(const QFileInfo &fileInfo){
	if( !m_pImage )
		m_pImage = new QImage();

	if( m_pImage->load( fileInfo.absoluteFilePath() ) ){
		//pewne flagi przyśpieszające malowanie
		setAttribute( Qt::WA_StaticContents );
		setAttribute( Qt::WA_OpaquePaintEvent );

		setRects( fileInfo );

		setFixedSize( m_pImage->size() );
		updateGeometry();
		update();
		return true;
	}
	setAttribute( Qt::WA_StaticContents, false );
	setAttribute( Qt::WA_OpaquePaintEvent, false  );
	delete m_pImage;
	m_pImage = NULL;
	return false;
}

void PaintArea::setRects(const QFileInfo &fi){
	QString path = fi.absolutePath();
	QString baseName = fi.baseName();

	QFile file( path + "/" + baseName + ".txt" );

	m_rects.clear();

	if( file.open(QIODevice::ReadOnly | QIODevice::Text) ){
		int i = 0;
		while( !file.atEnd() ){
			i++;
			QString line( file.readLine().trimmed() );
			QStringList list = line.split(" ");
			if( list.count() < 4){
				qWarning() << path + QDir::separator() + baseName + ".txt"
						   << " - w linii" << i-1 << "nieprawidłowa liczba"
							  " elementów.";
				continue;
			}else if( list.count() > 4 ){
				qWarning() << path + QDir::separator() + baseName + ".txt"
						   << " - w linii" << i-1 << "jest za dużo elementów, "
							  "zostaną wykorzystane tylko pierwsze cztery.";
			}
			m_rects.push_back(QRect(list.at(0).toInt(),
									list.at(1).toInt(),
									list.at(2).toInt() - list.at(0).toInt(),
									list.at(3).toInt() - list.at(1).toInt()));

		}
	}else{
		qWarning() << "Nie udało się otworzyć pliku txt: "
				   <<  path + "/" + baseName + ".txt";
	}
}
