#ifndef GUI_H
#define GUI_H

#include <QtGui/QMainWindow>
#include <qtextcodec.h>
#include <qtextobject.h>
#include <qtextdocument.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qgraphicsscene.h>
#include <qstring.h>
#include <qvector.h>
#include <process.h>
#include "ui_gui.h"

#define DOT_EXE_FILENAME "C:\\Program Files (x86)\\Graphviz 2.28\\bin\\dot.exe"

class GUI : public QMainWindow
{
	Q_OBJECT

public:
	GUI(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GUI();

private:
	Ui::GUIClass ui;
	QString appPath;			// Абсолютный путь до исполняемого файла.
	QVector<QString> images;	// Сгенерированные картинки для текущего введенного графа.
	int currentImage;			// Индекс текущей картинки.
	QGraphicsScene scene;		// Для показа картинок.

private slots:
	void btnSearch_clicked(bool checked);
	void btnPrevious_clicked(bool checked);
	void btnNext_clicked(bool checked);
};

#endif // GUI_H
