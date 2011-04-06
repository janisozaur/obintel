#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setApplicationName("faces");
	QCoreApplication::setOrganizationDomain("wacjan.com");
	QCoreApplication::setOrganizationName("wacjan");
	MainWindow w;
	w.show();

	return a.exec();
}
