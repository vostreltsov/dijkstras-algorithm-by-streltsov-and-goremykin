#include "gui.h"

GUI::GUI(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));
	ui.setupUi(this);
	connect(ui.btnSearch, SIGNAL(clicked(bool)), this, SLOT(btnSearch_clicked(bool)));
	connect(ui.btnPrevious, SIGNAL(clicked(bool)), this, SLOT(btnPrevious_clicked(bool)));
	connect(ui.btnNext, SIGNAL(clicked(bool)), this, SLOT(btnNext_clicked(bool)));
	appPath = QCoreApplication::applicationDirPath() + "/";
	QSettings settings(appPath + QString("settings.ini"), QSettings::IniFormat);
	dotExeFileName = settings.value("Main/dotpath", "C:/Program Files (x86)/Graphviz 2.28/bin/dot.exe").toString();
}

GUI::~GUI()
{
	// Удаляем сгенерированные картинки.
	for (QVector<QString>::const_iterator iter = images.constBegin(); iter != images.constEnd(); iter++)
		_unlink((*iter).toLocal8Bit().data());
}

void GUI::btnSearch_clicked(bool checked)
{
	// Проверка входных данных на соответствие формату.
	QRegExp regex("\\s*(\\d+)\\s*(\\d+)\\s*(\\d+)\\s*");
	QTextDocument * doc = ui.teGraph->document();
	bool failed = false;
	int maxVertex = -1;
	for (int i = 0; !failed && i < doc->lineCount(); i++)
	{
		QString line = doc->findBlockByLineNumber(i).text();
		if (regex.exactMatch(line))
		{
			int v1 = regex.cap(1).toInt();
			int v2 = regex.cap(2).toInt();
			if (v1 > maxVertex)
				maxVertex = v1;
			if (v2 > maxVertex)
				maxVertex = v2;
		}
		else
			failed = true;
	}
	// Если ошибок нет - удаляем старые картинки и генерируем новые.
	if (!failed)
	{
		for (QVector<QString>::const_iterator iter = images.constBegin(); iter != images.constEnd(); iter++)
			_unlink((*iter).toLocal8Bit().data());
		images.clear();
		currentImage = 0;
		// Генерация входного файла.
		QString inputFileName = appPath + QString("in.txt");
		FILE * file;
		if (fopen_s(&file, inputFileName.toLocal8Bit().data(), "w") != 0)
		{
			QMessageBox::warning(NULL, QString("Ошибка"), QString("Что-то пошло не так"));
			return;
		}
		fprintf_s(file, "%d %d %d\n", maxVertex + 1, ui.leStartVertex->text().toInt(), ui.leEndVertex->text().toInt());
		for (int i = 0; !failed && i < doc->lineCount(); i++)
		{
			char buf[256];
			QString line = doc->findBlockByLineNumber(i).text();
			strcpy(buf, line.toLocal8Bit().data());
			if (i != doc->lineCount() - 1)
				fprintf_s(file, "%s\n", buf);
			else
				fprintf_s(file, "%s", buf);
		}
		fclose(file);
		// Запуск алгоритма.
		QString daExeFileName = appPath + QString("DijkstrasAlgorithm.exe");
		QString outputFileName = appPath + QString("out.txt");
		QString prefixFileName = appPath + QString("step");
		QStringList args;
		args << inputFileName << outputFileName << prefixFileName;
		QProcess::execute(daExeFileName, args);
		_unlink(inputFileName.toLocal8Bit().data());
		// Подготовка данных для визуализации.
		if (fopen_s(&file, outputFileName.toLocal8Bit().data(), "r") != 0)
		{
			QMessageBox::warning(NULL, QString("Ошибка"), QString("Что-то пошло не так"));
			return;
		}
		while (!feof(file))
		{
			char dotFileName[256] = "";
			char tmp = '\0';
			int len = 0;
			while (tmp != '\n' && !feof(file))
			{
				tmp = fgetc(file);
				dotFileName[len] = tmp;
				len++;
			}
			dotFileName[len - 1] = '\0';
			QStringList args;
			args << QString("-Tpng") << QString("-o") + QString(dotFileName) + QString(".png") << QString(dotFileName);
			QProcess::execute(dotExeFileName, args);
			_unlink(dotFileName);
			images.push_back(QString(dotFileName) + QString(".png"));

		}
		fclose(file);
		_unlink(outputFileName.toLocal8Bit().data());
		// Показываем начальный шаг.
		ui.labGraphImage->setPixmap(QPixmap(images[0]));
	}
	else
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Список дуг не соответствует формату x y w"));
}

void GUI::btnPrevious_clicked(bool checked)
{
	if (images.empty())
		return;
	currentImage--;
	if (currentImage < 0)
		currentImage = images.size() - 1;
	ui.labGraphImage->setPixmap(QPixmap(images[currentImage]));

}

void GUI::btnNext_clicked(bool checked)
{
	if (images.empty())
		return;
	currentImage++;
	if (currentImage == images.size())
		currentImage = 0;
	ui.labGraphImage->setPixmap(QPixmap(images[currentImage]));
}