#include "gui.h"

GUI::GUI(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));
	ui.setupUi(this);
	connect(ui.btnSearch, SIGNAL(clicked(bool)), this, SLOT(btnSearch_clicked(bool)));
	connect(ui.btnPrevious, SIGNAL(clicked(bool)), this, SLOT(btnPrevious_clicked(bool)));
	connect(ui.btnNext, SIGNAL(clicked(bool)), this, SLOT(btnNext_clicked(bool)));

	connect(ui.btnMenuOpen, SIGNAL(triggered(bool)), this, SLOT(btnMenuOpen_triggered(bool)));
	connect(ui.btnMenuSave, SIGNAL(triggered(bool)), this, SLOT(btnMenuSave_triggered(bool)));
	connect(ui.btnMenuExit, SIGNAL(triggered(bool)), this, SLOT(btnMenuExit_triggered(bool)));
	connect(ui.btnMenuHelp, SIGNAL(triggered(bool)), this, SLOT(btnMenuHelp_triggered(bool)));
	connect(ui.btnMenuAbout, SIGNAL(triggered(bool)), this, SLOT(btnMenuAbout_triggered(bool)));

	appPath = QCoreApplication::applicationDirPath() + "/";
	QSettings settings(appPath + QString("settings.ini"), QSettings::IniFormat);
	dotExeFileName = settings.value("Main/dotpath", "C:/Program Files (x86)/Graphviz 2.28/bin/dot.exe").toString();
	if (dotExeFileName[1] != QChar(':'))
		dotExeFileName = appPath + dotExeFileName;
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
	QRegExp regex("\\s*(\\d+)\\s+(\\d+)\\s+(-?\\d+)\\s*");	// Валидатор для формата дуг.
	QTextDocument * doc = ui.teGraph->document();			// Содержимое TextEdit.
	QList<QString> lines;									// Непустые строки из содержимого TextEdit.
	bool failed = false;									// Найдены ли ошибки.
	int maxVertex = -1;										// Максимальный встретившийся номер вершины.

	for (int i = 0; !failed && i < doc->lineCount(); i++)
	{
		QString line = doc->findBlockByLineNumber(i).text();
		if (line != QString(""))
		{
			// Строка должна соответствовать шаблону \s*(\d+)\s+(\d+)\s+(-?\d)\s*
			if (regex.exactMatch(line))
			{
				int v1 = regex.cap(1).toInt();
				int v2 = regex.cap(2).toInt();
				if (v1 > maxVertex)
					maxVertex = v1;
				if (v2 > maxVertex)
					maxVertex = v2;
				lines << line;
			}
			else
				failed = true;
		}
	}

	// Если найдены ошибки в формате - выходим.
	if (failed)
	{
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Список дуг не соответствует формату x y w."));
		return;
	}

	// Удаляем старые картинки.
	for (QVector<QString>::const_iterator iter = images.constBegin(); iter != images.constEnd(); iter++)
		_unlink((*iter).toLocal8Bit().data());
	images.clear();
	currentImage = 0;

	// Генерация входного файла для консольного приложения с алгоритмом.
	QString inputFileName = appPath + QString("in.txt");
	FILE * file;
	if (fopen_s(&file, inputFileName.toLocal8Bit().data(), "w") != 0)
	{
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Не удалось сгенерировать входной файл для работы алгоритма."));
		return;
	}
	fprintf_s(file, "%d\n %d %d %d\n", (int)lines.size(), maxVertex + 1, ui.leStartVertex->text().toInt(), ui.leEndVertex->text().toInt());
	for (QList<QString>::const_iterator iter = lines.constBegin(); iter != lines.constEnd(); iter++)
		fprintf_s(file, "%s\n", iter->toLocal8Bit().data());
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
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Не удалось открыть файл с результатами работы алгоритма."));
		return;
	}
	char buf[256] = "";
	int linesCount = 0;
	fgets(buf, 256, file);
	fscanf_s(file, "%d\n", &linesCount);
	buf[strlen(buf) - 1] = '\0';
	if (strcmp(buf, "fail") == 0)
	{
		// Выводим сообщение об ошибках.
		QString errors;
		for (int i = 0; i < linesCount; i++)
		{
			fgets(buf, 256, file);
			errors += QString(buf);
			if (i != linesCount - 1)
				errors += QString("\n");
		}
		QMessageBox::warning(NULL, QString("Ошибки во входных данных."), errors);
		ui.labGraphImage->clear();
	}
	else
	{
		// Визуализируем шаги алгоритма.
		for (int i = 0; i < linesCount; i++)
		{
			fgets(buf, 256, file);
			buf[strlen(buf) - 1] = '\0';
			QStringList args;
			args << QString("-Tpng") << QString("-o") + QString(buf) + QString(".png") << QString(buf);
			QProcess::execute(dotExeFileName, args);
			_unlink(buf);
			images.push_back(QString(buf) + QString(".png"));
		}
		// Показываем начальный шаг.
		ui.labGraphImage->setPixmap(QPixmap(images[0]));
	}
	fclose(file);
	_unlink(outputFileName.toLocal8Bit().data());
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

void GUI::btnMenuOpen_triggered(bool checked)
{
	QString fileName = QFileDialog::getOpenFileName(this, QString("Открыть граф"), QString(""), QString("Файл описания графа (*.graph)"), 0, 0);
	if (fileName == QString(""))
		return;
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Не удалось открыть файл."));
		return;
	}
	QTextStream stream(&file);
	QString newText;
	int cnt = stream.readLine().toInt();
	for (int i = 0; i < cnt; i++)
		newText.append(stream.readLine() + QString("\n"));
	ui.teGraph->document()->setPlainText(newText);
	file.close();
}

void GUI::btnMenuSave_triggered(bool checked)
{
	QString fileName = QFileDialog::getSaveFileName(this, QString("Сохранить граф"), QString(""), QString("Файл описания графа (*.graph)"), 0, 0);
	if (fileName == QString(""))
		return;
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Не удалось сохранить файл."));
		return;
	}
	QTextStream stream(&file);
	QTextDocument * doc = ui.teGraph->document();	// Содержимое TextEdit.
	QList<QString> lines;							// Непустые строки из содержимого TextEdit.
	for (int i = 0; i < doc->lineCount(); i++)
	{
		QString line = doc->findBlockByLineNumber(i).text();
		if (line != QString(""))
			lines << line;
	}
	stream << QString::number(lines.size(), 10) + QString("\n");
	for (QList<QString>::const_iterator iter = lines.constBegin(); iter != lines.constEnd(); iter++)
		stream << *iter + QString("\n");
	file.close();
}

void GUI::btnMenuExit_triggered(bool checked)
{
	close();
}

void GUI::btnMenuHelp_triggered(bool checked)
{

}

void GUI::btnMenuAbout_triggered(bool checked)
{

}