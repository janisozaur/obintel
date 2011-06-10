#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QEvent>
#include "paintarea.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
	Ui::MainWindow *ui;
	QFileSystemModel *m_pModel;
	PaintArea *m_pPaintArea;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
	bool eventFilter(QObject *obj, QEvent *event);

	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
private slots:
	void openDir();
	void loadImage(QModelIndex);

};

#endif // MAINWINDOW_H

