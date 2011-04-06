#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStringList>
#include <QKeySequence>
#include <QFileInfo>
#include <QList>
#include <QUrl>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_pModel(NULL),
	m_pPaintArea(NULL)
{
    ui->setupUi(this);
	ui->actionExit->setShortcut( QKeySequence::Quit );
	ui->actionOpenDir->setShortcut( QKeySequence::Open );

	setWindowTitle( tr("OI - zad1 - przeglądarka") );

	m_pModel = new QFileSystemModel();
	m_pModel->setNameFilterDisables ( false );

	QStringList filters;
	filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";

	m_pModel->setNameFilters( filters );

	m_pPaintArea = new PaintArea( ui->scrollArea );
	ui->scrollArea->setWidget( m_pPaintArea );

	connect( ui->actionOpenDir, SIGNAL(triggered()),
			 this, SLOT(openDir()) );
	connect( ui->actionExit, SIGNAL(triggered()),
			 qApp, SLOT(quit()) );

	connect( ui->treeView, SIGNAL(activated(QModelIndex)),
			 this, SLOT(loadImage(QModelIndex)) );

	ui->treeView->installEventFilter( this );
	ui->scrollArea->installEventFilter( this );
	m_pPaintArea->installEventFilter( this );

	ui->treeView->setAcceptDrops( false );
	ui->scrollArea->setAcceptDrops( false );
	m_pPaintArea->setAcceptDrops( false );

	this->setAcceptDrops( true );
}

MainWindow::~MainWindow(){
    delete ui;
	delete m_pModel;
}

void MainWindow::openDir(){
	QString dir =
			QFileDialog::getExistingDirectory( this, tr("Otwórz katalog") );

	if( dir.isEmpty() )
		return;

	m_pModel->setRootPath( dir );

	ui->treeView->setModel( m_pModel );
	ui->treeView->setRootIndex( m_pModel->index(dir) );

	ui->treeView->setSortingEnabled( true );
	ui->treeView->sortByColumn( 0, Qt::AscendingOrder );
	ui->treeView->hideColumn( 1 );
	ui->treeView->hideColumn( 2 );
	ui->treeView->hideColumn( 3 );

}

void MainWindow::loadImage(QModelIndex index){
	QFileInfo fi = m_pModel->fileInfo( index );

	if( !m_pPaintArea->load( fi ) ){
		statusBar()->showMessage( tr("Nie udało się otworzyć pliku"), 5000 );
	}
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event){
	if( event->type() == QEvent::KeyPress ){
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if( keyEvent->key() == Qt::Key_Space ){
			QKeyEvent firstEvent(QEvent::KeyPress, Qt::Key_Down,
								 Qt::NoModifier);
			QApplication::sendEvent( ui->treeView, &firstEvent);
			QKeyEvent secondEvent(QEvent::KeyPress, Qt::Key_Enter,
								  Qt::NoModifier);
			QApplication::sendEvent( ui->treeView, &secondEvent);
			qApp->processEvents();
			return true;
		}else if( keyEvent->key() == Qt::Key_Backspace ){
			QKeyEvent firstEvent(QEvent::KeyPress, Qt::Key_Up,
								 Qt::NoModifier);
			QApplication::sendEvent( ui->treeView, &firstEvent);
			QKeyEvent secondEvent(QEvent::KeyPress, Qt::Key_Enter,
								  Qt::NoModifier);
			QApplication::sendEvent( ui->treeView, &secondEvent);
			qApp->processEvents();
			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e){
	if( e->mimeData()->hasFormat("text/uri-list") )
		e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *e){
	QList<QUrl> urls = e->mimeData()->urls();

	if( urls.isEmpty() )
		return;

	QString dir = urls.first().toLocalFile();
	if( dir.isEmpty() )
		return;

	QFileInfo fi( dir );
	if( !fi.isDir() )
		return;

	m_pModel->setRootPath( dir );

	ui->treeView->setModel( m_pModel );
	ui->treeView->setRootIndex( m_pModel->index(dir) );

	ui->treeView->setSortingEnabled( true );
	ui->treeView->sortByColumn( 0, Qt::AscendingOrder );
	ui->treeView->hideColumn( 1 );
	ui->treeView->hideColumn( 2 );
	ui->treeView->hideColumn( 3 );
}
