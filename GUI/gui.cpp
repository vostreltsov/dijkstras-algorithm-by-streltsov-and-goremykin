#include "gui.h"

ScalableGraphicsView::ScalableGraphicsView(QWidget * parent)
{
}

ScalableGraphicsView::~ScalableGraphicsView()
{
}		

void ScalableGraphicsView::wheelEvent(QWheelEvent * event)
{
	if (event->modifiers() == Qt::ControlModifier)
		if (event->delta() > 0)
			scale(1.25, 1.25);
		else
			scale(0.8, 0.8);
	else
		QGraphicsView::wheelEvent(event);
}

GUI::GUI(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));
	ui.setupUi(this);
	validator.setRegExp(QRegExp("[^ ]+"));
	ui.leStartVertex->setValidator(&validator);
	ui.leEndVertex->setValidator(&validator);
	enableButtons(false, false, false, false);

	scene = NULL;
	gvGraph = new ScalableGraphicsView(parent);
	gvGraph->setScene(NULL);
	gvGraph->setGeometry(10, 23, 662, 415);
	gvGraph->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	gvGraph->setAcceptDrops(false);
	gvGraph->setAutoFillBackground(true);
	gvGraph->setDragMode(QGraphicsView::ScrollHandDrag);
	
	gvLayout = new QGridLayout(parent);
	gvLayout->addWidget(gvGraph, 0, 0, 1, 5);
	gvLayout->addWidget(ui.btnSearch, 1, 0);
	gvLayout->addWidget(ui.btnToTheBeginning, 1, 1);
	gvLayout->addWidget(ui.btnPrevious, 1, 2);
	gvLayout->addWidget(ui.btnNext, 1, 3);
	gvLayout->addWidget(ui.btnToTheEnd, 1, 4);
	ui.gbVisualization->setLayout(gvLayout);
	
	connect(ui.btnShowGraph, SIGNAL(clicked(bool)), this, SLOT(btnShowGraph_clicked(bool)));
	connect(ui.btnSearch, SIGNAL(clicked(bool)), this, SLOT(btnSearch_clicked(bool)));	
	connect(ui.btnToTheBeginning, SIGNAL(clicked(bool)), this, SLOT(btnToTheBeginning_clicked(bool)));
	connect(ui.btnPrevious, SIGNAL(clicked(bool)), this, SLOT(btnPrevious_clicked(bool)));
	connect(ui.btnNext, SIGNAL(clicked(bool)), this, SLOT(btnNext_clicked(bool)));
	connect(ui.btnToTheEnd, SIGNAL(clicked(bool)), this, SLOT(btnToTheEnd_clicked(bool)));

	connect(ui.btnMenuOpen, SIGNAL(triggered(bool)), this, SLOT(btnMenuOpen_triggered(bool)));
	connect(ui.btnMenuSave, SIGNAL(triggered(bool)), this, SLOT(btnMenuSave_triggered(bool)));
	connect(ui.btnMenuCreateReport, SIGNAL(triggered(bool)), this, SLOT(btnMenuCreateReport_triggered(bool)));
	
	connect(ui.btnMenuExit, SIGNAL(triggered(bool)), this, SLOT(btnMenuExit_triggered(bool)));
	connect(ui.btnMenuHelp, SIGNAL(triggered(bool)), this, SLOT(btnMenuHelp_triggered(bool)));
	connect(ui.btnMenuAlgorithm, SIGNAL(triggered(bool)), this, SLOT(btnMenuAlgorithm_triggered(bool)));
	connect(ui.btnMenuAbout, SIGNAL(triggered(bool)), this, SLOT(btnMenuAbout_triggered(bool)));

	// Загружаем настройки.
	appPath = QCoreApplication::applicationDirPath() + QDir::separator();
	dotExeFileName = "";
	dotPathSetManually = false;
	if (QFile::exists(appPath + QString("settings.ini")))
	{
		QSettings settings(appPath + QString("settings.ini"), QSettings::IniFormat);
		dotExeFileName = settings.value("Main/dotpath", "").toString();
		if (dotExeFileName.length() > 0 && dotExeFileName[1] != QChar(':'))
			dotExeFileName = appPath + dotExeFileName;
	}
	if (!QFile::exists(dotExeFileName))
	{
		QMessageBox::warning(NULL, QString("Предупреждение"), QString("Не найден файл dot.exe библиотеки GraphViz. Укажите его местоположение."));
		dotExeFileName = QFileDialog::getOpenFileName(this, QString("Местоположение файла dot.exe"), QString(""), QString("dot.exe"), 0, 0);
		dotPathSetManually = true;
	}
	qsrand((unsigned int)time(NULL));
}

GUI::~GUI()
{
	cleanUp();
	if (scene != NULL)
		delete scene;
	layout()->removeWidget(gvGraph);
	delete gvGraph;
	delete gvLayout;
	// Сохраняем настройки.
	QSettings settings(appPath + QString("settings.ini"), QSettings::IniFormat);
	if (dotPathSetManually)
		settings.setValue(QString("Main/dotpath"), dotExeFileName);
}

bool GUI::isValid()
{
	return QFile::exists(dotExeFileName);
}

bool GUI::validateFormat(QList<QString> * lines)
{
	QRegExp regex("\\s*([^ ]+)\\s+([^ ]+)\\s+(-?\\d+)\\s*");
	QTextDocument * doc = ui.teGraph->document();
	bool failed = false;

	for (int i = 0; i < doc->lineCount(); i++)
	{
		QString line = doc->findBlockByLineNumber(i).text();
		if (line != QString(""))
		{
			// Строка должна соответствовать шаблону \s*([^ ]+)\s+([^ ]+)\s+(-?\d)\s*
			if (regex.exactMatch(line))
			{
				if (lines != NULL)
					*lines << line;
			}
			else
				failed = true;
		}
	}
	return !failed;
}

void GUI::cleanUp()
{
	// Удаляем сгенерированные картинки.
	for (QVector<QString>::const_iterator iter = images.constBegin(); iter != images.constEnd(); iter++)
		_unlink((*iter).toLocal8Bit().data());
	images.clear();
	currentImage = 0;
}

void GUI::enableButtons(bool beginning, bool previous, bool next, bool end)
{
	ui.btnToTheBeginning->setEnabled(beginning);
	ui.btnPrevious->setEnabled(previous);
	ui.btnNext->setEnabled(next);	
	ui.btnToTheEnd->setEnabled(end);
}

bool GUI::removeDir(const QString & dirName)
{
	QDir dir(dirName);
	if (!dir.exists())
		return true;

	bool result = true;
	Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
	{
		if (info.isDir())
			result = removeDir(info.absoluteFilePath());
		else
			result = QFile::remove(info.absoluteFilePath());
		if (!result)
			return false;
	}
	result = dir.rmdir(dirName);
	return result;
}

void GUI::btnShowGraph_clicked(bool checked)
{
	QList<QString> lines;	// Непустые строки из содержимого TextEdit.

	enableButtons(false, false, false, false);
	if (scene != NULL)
		delete scene;
	scene = new QGraphicsScene(parent());
	gvGraph->setScene(scene);

	// Если найдены ошибки в формате - выходим.
	if (!validateFormat(&lines))
	{
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Список дуг не соответствует формату x y w."));
		return;
	}
	if (lines.size() == 0)
		return;

	// Генерируем dot-файл.
	QString tmpFileName = appPath + QString("tmp.dot");
	FILE * file;
	if (fopen_s(&file, tmpFileName.toLocal8Bit().data(), "w"))
		return;
	fprintf_s(file, "digraph {\nrankdir = LR;\n");
	for (QList<QString>::const_iterator iter = lines.constBegin(); iter != lines.constEnd(); iter++)
	{
		QRegExp regex("\\s*([^ ]+)\\s+([^ ]+)\\s+(-?\\d+)\\s*");
		regex.exactMatch(*iter);
		fprintf_s(file, "\"%s\" -> \"%s\" [label=\"%I64d\"];\n", regex.cap(1).toLocal8Bit().data(), regex.cap(2).toLocal8Bit().data(), regex.cap(3).toLongLong());
	}
	fprintf_s(file, "};");
	fclose(file);

	QStringList args;
	args << QString("-Tpng") << QString("-o") + tmpFileName + QString(".png") << tmpFileName;
	QProcess::execute(dotExeFileName, args);
	scene->addPixmap(QPixmap(tmpFileName + QString(".png")));
	_unlink(tmpFileName.toLocal8Bit().data());
	_unlink((tmpFileName + QString(".png")).toLocal8Bit().data());
	cleanUp();
}

void GUI::btnSearch_clicked(bool checked)
{
	QList<QString> lines;	// Непустые строки из содержимого TextEdit.

	enableButtons(false, false, false, false);
	if (scene != NULL)
		delete scene;
	scene = new QGraphicsScene(parent());
	gvGraph->setScene(scene);

	// Если найдены ошибки в формате - выходим.
	if (!validateFormat(&lines))
	{
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Список дуг не соответствует формату x y w."));
		return;
	}
	if (lines.size() == 0)
		return;
	if (ui.leStartVertex->text() == ui.leEndVertex->text())
	{
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Начальная и конечная вершины маршрута совпадают."));
		return;
	}

	cleanUp();
	lastRoute[0] = ui.leStartVertex->text();
	lastRoute[1] = ui.leEndVertex->text();

	// Генерация входного файла для консольного приложения с алгоритмом.
	QString inputFileName = appPath + QString("in.txt");
	FILE * file;
	if (fopen_s(&file, inputFileName.toLocal8Bit().data(), "w") != 0)
	{
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Не удалось сгенерировать входной файл для работы алгоритма."));
		return;
	}
	fprintf_s(file, "%I64d\n%s %s\n", (__int64)lines.size(), lastRoute[0].toLocal8Bit().data(), lastRoute[1].toLocal8Bit().data());
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
	statusBar()->showMessage(QString("Выполнение алгоритма..."));
	if (fopen_s(&file, outputFileName.toLocal8Bit().data(), "r") != 0)
	{
		statusBar()->showMessage(QString("Не удалось открыть файл с результатами работы алгоритма."));
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
		statusBar()->showMessage(QString(""));
		QMessageBox::warning(NULL, QString("Ошибки во входных данных."), errors);
	}
	else
	{
		int pathSize = 0;
		fscanf_s(file, "%d\n", &pathSize);
		// Визуализируем шаги алгоритма.
		for (int i = 0; i < linesCount; i++)
		{
			statusBar()->showMessage(QString("Подготовка шага алгоритма: ") + QString::number(i + 1) + QString(" из ") + QString::number(linesCount));
			fgets(buf, 256, file);
			buf[strlen(buf) - 1] = '\0';
			QStringList args;
			args << QString("-Tpng") << QString("-o") + QString(buf) + QString(".png") << QString(buf);
			QProcess::execute(dotExeFileName, args);
			_unlink(buf);
			images.push_back(QString(buf) + QString(".png"));
		}
		if (pathSize == 0)
			statusBar()->showMessage(QString("Путь не найден."));
		else
			statusBar()->showMessage(QString("Путь найден"));
		// Показываем начальный шаг.
		scene->addPixmap(QPixmap(images[0]));
		btnToTheBeginning_clicked(false);
	}
	fclose(file);
	_unlink(outputFileName.toLocal8Bit().data());
}

void GUI::btnToTheBeginning_clicked(bool checked)
{
	if (images.empty())
		return;
	currentImage = 0;
	if (scene != NULL)
		delete scene;
	scene = new QGraphicsScene(parent());
	gvGraph->setScene(scene);
	scene->addPixmap(QPixmap(images[currentImage]));
	enableButtons(true, false, true, true);
}

void GUI::btnPrevious_clicked(bool checked)
{
	if (images.empty())
		return;
	currentImage--;
	if (scene != NULL)
		delete scene;
	scene = new QGraphicsScene(parent());
	gvGraph->setScene(scene);
	scene->addPixmap(QPixmap(images[currentImage]));
	enableButtons(true, currentImage > 0, true, true);
}

void GUI::btnNext_clicked(bool checked)
{
	if (images.empty())
		return;
	currentImage++;
	if (scene != NULL)
		delete scene;
	scene = new QGraphicsScene(parent());
	gvGraph->setScene(scene);
	scene->addPixmap(QPixmap(images[currentImage]));
	enableButtons(true, true, currentImage < images.size() - 1, true);
}

void GUI::btnToTheEnd_clicked(bool checked)
{
	if (images.empty())
		return;
	currentImage = images.size() - 1;
	if (scene != NULL)
		delete scene;
	scene = new QGraphicsScene(parent());
	gvGraph->setScene(scene);
	scene->addPixmap(QPixmap(images[currentImage]));
	enableButtons(true, true, false, true);
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

void GUI::btnMenuCreateReport_triggered(bool checked)
{
	if (images.size() < 2)
	{
		QMessageBox::information(NULL, QString("Нет данных для отчета"), QString("Для создания отчета необходимо сначала найти кратчайший путь."));
		return;
	}
	QString fileName = QFileDialog::getSaveFileName(this, QString("Сохранить отчет"), QString(""), QString("Файл html (*.html)"), 0, 0);
	if (fileName == QString(""))
		return;
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(NULL, QString("Ошибка"), QString("Не удалось сохранить файл."));
		return;
	}
	QTextStream stream(&file);
	// Копируем сгенерированные картинки, случайным образом генерируя имена файлов.
	QVector<QString> pngFileNames;
	QFileInfo info(fileName);
	QString basename = info.baseName();
	QString dir = info.absoluteDir().absolutePath() + QDir::separator();
	QString suffix = basename + QString(".files") + QDir::separator();
	QDir tmp(dir);
	dir += suffix;
	removeDir(dir);
	tmp.mkdir(suffix);
	
	
	for (QVector<QString>::const_iterator iter = images.constBegin(); iter != images.constEnd(); iter++)
	{
		QString fileName("");
		do
		{
			for (int i = 0; i < 32; i++)
				if (qrand() % 2)
					fileName += QChar('a' + qrand() % 26);
				else
					fileName += QChar('0' + qrand() % 10);
		}
		while (QFile::exists(dir + fileName + QString(".png")));
		fileName = dir + fileName + QString(".png");
		QFile::copy(*iter, fileName);
		pngFileNames.push_back(fileName);
	}

	stream << QString("<html>\n	<body>\n");
	stream << QString("		Поиск кратчайшего марштура из вершины <b>") + lastRoute[0] + QString("</b> в вершину <b>") + lastRoute[1] + QString("</b>:<br/>\n");
	stream << QString("		<img src=\"") + pngFileNames.first() + QString("\"><br/>\n");
	for (int i = 1; i < pngFileNames.size() - 1; i++)
	{
		stream << QString("		Шаг ") + QString::number(i) + QString(":<br/>\n");
		stream << QString("		<img src=\"") + pngFileNames[i] + QString("\"><br/>\n");
	}
	stream << QString("		Результат:<br/>\n");
	stream << QString("		<img src=\"") + pngFileNames.last() + QString("\"><br/>\n");
	stream << QString("	</body>\n</html>");
	file.close();
}

void GUI::btnMenuExit_triggered(bool checked)
{
	close();
}

void GUI::btnMenuHelp_triggered(bool checked)
{
	QMessageBox::information(NULL, QString("Использование программы"),	QString("Граф представляется списком дуг, элемент списка имеет формат <вершина> <вершина> <вес>\n") +
																		QString("Например, список дуг может иметь вид:\na0 a1 3\na1 a2 2\n\n") +
																		QString("По нажатию на кнопку \"Показать граф\" появится графическое представление графа в области правее списка дуг.\n") +
																		QString("По нажатию на кнопку \"Найти кратчайший путь\" будет выполнен алгоритм Дейкстры, шаги которого можно просмотреть в графическом виде.\n") +
																		QString("Кнопки \"В начало\", \"Предыдущий шаг\", \"Следующий шаг\" и \"В конец\" позволяют прокручивать шаги алгоритма.\n\n") +
																		QString("Вершины графа имеют имена, при каждой вершине хранится метка len, равная длине пути от начальной вершины до нее. Метка len=-1 означает бесконечную длину пути. ") +
																		QString("Текущая просматриваемая дуга выделяется красным цветом, просмотренные вершины выделяются пунктиром, а выходящие из них дуги - синим цветом. ") +
																		QString("На последнем шаге дуги, принадлежащие найденному пути, выделяются фиолетовым цветом.\n") +
																		QString("Граф можно масштабировать с помощью колеса мыши при зажатой клавише Ctrl.\n") +
																		QString("Возможно формирование отчета в формате html. Для этого необходимо в главном меню выбрать пункт Файл->Создать отчет в формате html.\n"));
}

void GUI::btnMenuAlgorithm_triggered(bool checked)
{
	QMessageBox::information(NULL, QString("Об алгоритме"),	QString("Алгоритм Дейкстры — алгоритм на графах, изобретенный нидерландским ученым Э. Дейкстрой в 1959 году. Находит кратчайшее расстояние от одной из вершин графа до ") +
															QString("всех остальных. Алгоритм работает только для графов без дуг с отрицательным весом и петель.\n\n") +
															QString("Инициализация: Метка начальной вершины маршрута полагается равной 0, метки остальных вершин — бесконечности. Это отражает то, что расстояния от начальной вершины до других вершин пока неизвестны. Все вершины графа помечаются как непосещенные.\n\n") +
															QString("Шаг алгоритма: Если все вершины посещены, алгоритм завершается. В противном случае, из еще не посещенных вершин выбирается вершина u, имеющая минимальную метку. Вершины, в которые ведут ребра из u, назовем соседями этой вершины. ") +
															QString("Для каждого соседа вершины u, кроме отмеченных как посещенные, рассмотрим новую длину пути, равную сумме значений текущей метки u и длины ребра, соединяющего u с этим соседом. ") +
															QString("Если полученное значение длины меньше значения метки соседа, заменим значение метки полученным значением длины. Рассмотрев всех соседей, пометим вершину u как посещенную и повторим шаг алгоритма."));
}

void GUI::btnMenuAbout_triggered(bool checked)
{
	QMessageBox::information(NULL, QString("О программе"), QString("<p align='center'>Визуализация работы алгоритма Дейкстры.<br><br>Авторы: студенты группы ИВТ-360 Стрельцов В. О., Горемыкин М. В.<br><br>Волгорадский Государственный Технический Университет</center><br>Кафедра ПОАС<br>2012 год</p>"));
}