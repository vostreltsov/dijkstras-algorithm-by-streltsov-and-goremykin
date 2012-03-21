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
		char inputFileName[256];
		strcpy(inputFileName, appPath.toLocal8Bit().data());
		strcat(inputFileName, "in.txt");
		FILE * file;
		if (fopen_s(&file, inputFileName, "w") != 0)
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
		char daExeFileName[256];
		char outputFileName[256];
		char prefixFileName[256];
		strcpy(daExeFileName, appPath.toLocal8Bit().data());
		strcpy(outputFileName, daExeFileName);
		strcpy(prefixFileName, daExeFileName);
		strcat(daExeFileName, "DijkstrasAlgorithm.exe");
		strcat(outputFileName, "out.txt");
		strcat(prefixFileName, "step");
		_spawnl(_P_WAIT, daExeFileName, "DijkstrasAlgorithm.exe", inputFileName, outputFileName, prefixFileName, NULL);
		_unlink(inputFileName);
		// Подготовка данных для визуализации.
		if (fopen_s(&file, outputFileName, "r") != 0)
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
			char param1[256] = "";
			char param2[256] = "";
			char pngFileName[256] = "";
			sprintf_s(param1, 256, "-o \"%s.png\"", dotFileName);
			sprintf_s(param2, 256, "-Kdot \"%s\"", dotFileName);
			sprintf_s(pngFileName, 256, "%s.png", dotFileName);
			_spawnl(_P_WAIT, DOT_EXE_FILENAME, "dot.exe", "-Tpng", param1, param2, NULL);
			_unlink(dotFileName);
			images.push_back(pngFileName);
		}
		fclose(file);
		_unlink(outputFileName);
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