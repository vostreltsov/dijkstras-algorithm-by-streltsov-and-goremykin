#include "gui.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GUI w;
	if (w.isValid())
	{
		w.show();
		return a.exec();
	}
	return 0;
}
