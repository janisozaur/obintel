#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QFormLayout>
#include <QLineEdit>
#include <QDebug>
#include <QDir>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QString fileName(QDir::homePath() + "/" + QCoreApplication::applicationName() + ".ini");
	qDebug() << "fileName" << fileName;
	QSettings settings(fileName, QSettings::IniFormat);
	QFormLayout *fl = qobject_cast<QFormLayout *>(ui->centralWidget->layout()->itemAt(0)->layout());
	qDebug() << fl;
	foreach (QString key, settings.childKeys()) {
		fl->addRow(key, new QLineEdit(settings.value(key).toString()));
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	QFormLayout *fl = qobject_cast<QFormLayout *>(ui->centralWidget->layout()->itemAt(0)->layout());
	QString fileName(QDir::homePath() + "/" + QCoreApplication::applicationName() + ".ini");
	qDebug() << "fileName" << fileName;
	QSettings settings(fileName, QSettings::IniFormat);
	for (int i = 0; i < fl->rowCount(); i++) {
		QLabel *keyLabel = qobject_cast<QLabel *>(fl->itemAt(i, QFormLayout::LabelRole)->widget());
		QString key = keyLabel->text();
		QLineEdit *valueEdit = qobject_cast<QLineEdit *>(fl->itemAt(i, QFormLayout::FieldRole)->widget());
		QString value = valueEdit->text();
		settings.setValue(key, value);
	}
	settings.sync();
}
