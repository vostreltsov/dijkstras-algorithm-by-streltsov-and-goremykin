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
#include <qprocess.h>
#include <qsettings.h>
#include "ui_gui.h"

class GUI : public QMainWindow
{
	Q_OBJECT

public:
	GUI(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GUI();

private:
	Ui::GUIClass ui;
	QString dotExeFileName;		// ���������� ���� � dot.exe.
	QString appPath;			// ���������� ���� �� ������������ �����.
	QVector<QString> images;	// ��������������� �������� ��� �������� ���������� �����.
	int currentImage;			// ������ ������� ��������.
	QGraphicsScene scene;		// ��� ������ ��������.

private slots:
	void btnSearch_clicked(bool checked);
	void btnPrevious_clicked(bool checked);
	void btnNext_clicked(bool checked);
	void btnMenuOpen_triggered(bool checked);
	void btnMenuSave_triggered(bool checked);
	void btnMenuExit_triggered(bool checked);
	void btnMenuHelp_triggered(bool checked);
	void btnMenuAbout_triggered(bool checked);

};

#endif // GUI_H