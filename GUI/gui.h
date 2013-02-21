#ifndef GUI_H
#define GUI_H

#include <time.h>
#include <QtGui/QMainWindow>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtextobject.h>
#include <qtextdocument.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qgraphicsscene.h>
#include <qstring.h>
#include <qvector.h>
#include <qprocess.h>
#include <qsettings.h>
#include <qiodevice.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfiledialog.h>
#include <qvalidator.h>
#include <qgraphicsview.h>
#include <qevent.h>
#include "ui_gui.h"

class ScalableGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	ScalableGraphicsView(QWidget * parent = 0);
	~ScalableGraphicsView();

protected:
	void wheelEvent(QWheelEvent * event);
};

class GUI : public QMainWindow
{
	Q_OBJECT

private:
	Ui::GUIClass ui;
	QString dotExeFileName;			// Абсолютный путь к dot.exe.
	QString appPath;				// Абсолютный путь до исполняемого файла.
	QVector<QString> images;		// Сгенерированные картинки для текущего введенного графа.
	int currentImage;				// Индекс текущей картинки.
	ScalableGraphicsView * gvGraph;	// Масштабируемый QGraphicsView.
	QGraphicsScene * scene;			// Для показа картинок.
	QGridLayout * gvLayout;			// Компоновщик для gvGraph.
	bool dotPathSetManually;		// Указан ли путь до dot.exe вручную.
	QRegExpValidator validator;		// Валидатор на вершины.
	QString lastRoute[2];			// Начало и конец найденного маршрута.

	bool validateFormat(QList<QString> * lines);
	void cleanUp();
	void enableButtons(bool beginning, bool previous, bool next, bool end);
	bool removeDir(const QString & dirName);

private slots:
	void btnShowGraph_clicked(bool checked);
	void btnSearch_clicked(bool checked);
	void btnToTheBeginning_clicked(bool checked);
	void btnPrevious_clicked(bool checked);
	void btnNext_clicked(bool checked);
	void btnToTheEnd_clicked(bool checked);
	void btnMenuOpen_triggered(bool checked);
	void btnMenuSave_triggered(bool checked);
	void btnMenuCreateReport_triggered(bool checked);
	void btnMenuExit_triggered(bool checked);
	void btnMenuHelp_triggered(bool checked);
	void btnMenuAlgorithm_triggered(bool checked);
	void btnMenuAbout_triggered(bool checked);

public:
	GUI(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GUI();
	bool isValid();

};

#endif // GUI_H