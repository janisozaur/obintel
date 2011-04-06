#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QWidget>
#include <QPaintEvent>
#include <QImage>
#include <QString>
#include <QFileInfo>
#include <QVector>
#include <QRect>

class PaintArea : public QWidget
{
    Q_OBJECT
private:
	/// @cond
	QImage *m_pImage;
	QVector<QRect> m_rects;
	/// @endcond

	void setRects(const QFileInfo &fi);
public:
	/// @cond
    explicit PaintArea(QWidget *parent = 0);
	~PaintArea();
	/// @endcond

	bool load(const QFileInfo &fileInfo);
protected:
	void paintEvent(QPaintEvent *event);
};

#endif // PAINTAREA_H
