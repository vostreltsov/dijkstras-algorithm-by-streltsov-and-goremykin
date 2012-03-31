#include "gui.h"

GUI::GUI(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));
	ui.setupUi(this);
	validator.setRegExp(QRegExp("\\d+"));
	ui.leStartVertex->setValidator(&validator);
	ui.leEndVertex->setValidator(&validator);
	ui.btnNext->setEnabled(false);
	ui.btnPrevious->setEnabled(false);
	connect(ui.btnShowGraph, SIGNAL(clicked(bool)), this, SLOT(btnShowGraph_clicked(bool)));
	connect(ui.btnSearch, SIGNAL(clicked(bool)), this, SLOT(btnSearch_clicked(bool)));
	connect(ui.btnPrevious, SIGNAL(clicked(bool)), this, SLOT(btnPrevious_clicked(bool)));
	connect(ui.btnNext, SIGNAL(clicked(bool)), this, SLOT(btnNext_clicked(bool)));

	connect(ui.btnMenuOpen, SIGNAL(triggered(bool)), this, SLOT(btnMenuOpen_triggered(bool)));
	connect(ui.btnMenuSave, SIGNAL(triggered(bool)), this, SLOT(btnMenuSave_triggered(bool)));
	connect(ui.btnMenuExit, SIGNAL(triggered(bool)), this, SLOT(btnMenuExit_triggered(bool)));
	connect(ui.btnMenuHelp, SIGNAL(triggered(bool)), this, SLOT(btnMenuHelp_triggered(bool)));
	connect(ui.btnMenuAlgorithm, SIGNAL(triggered(bool)), this, SLOT(btnMenuAlgorithm_triggered(bool)));
	connect(ui.btnMenuAbout, SIGNAL(triggered(bool)), this, SLOT(btnMenuAbout_triggered(bool)));

	// ��������� ���������.
	appPath = QCoreApplication::applicationDirPath() + "/";
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
		QMessageBox::warning(NULL, QString("��������������"), QString("�� ������ ���� dot.exe ���������� GraphViz. ������� ��� ��������������."));
		dotExeFileName = QFileDialog::getOpenFileName(this, QString("�������������� ����� dot.exe"), QString(""), QString("dot.exe"), 0, 0);
		dotPathSetManually = true;
	}
}

GUI::~GUI()
{
	cleanUp();
	// ��������� ���������.
	QSettings settings(appPath + QString("settings.ini"), QSettings::IniFormat);
	if (dotPathSetManually)
		settings.setValue(QString("Main/dotpath"), dotExeFileName);
}

bool GUI::isValid()
{
	return QFile::exists(dotExeFileName);
}

bool GUI::validateFormat(QList<QString> * lines, int * maxVertex)
{
	QRegExp regex("\\s*(\\d+)\\s+(\\d+)\\s+(-?\\d+)\\s*");
	QTextDocument * doc = ui.teGraph->document();
	bool failed = false;
	if (maxVertex != NULL)
		*maxVertex = -1;

	for (int i = 0; i < doc->lineCount(); i++)
	{
		QString line = doc->findBlockByLineNumber(i).text();
		if (line != QString(""))
		{
			// ������ ������ ��������������� ������� \s*(\d+)\s+(\d+)\s+(-?\d)\s*
			if (regex.exactMatch(line))
			{
				int v1 = regex.cap(1).toInt();
				int v2 = regex.cap(2).toInt();
				if (maxVertex != NULL && v1 > *maxVertex)
					*maxVertex = v1;
				if (maxVertex != NULL && v2 > *maxVertex)
					*maxVertex = v2;
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
	// ������� ��������������� ��������.
	for (QVector<QString>::const_iterator iter = images.constBegin(); iter != images.constEnd(); iter++)
		_unlink((*iter).toLocal8Bit().data());
	images.clear();
	currentImage = 0;
}

void GUI::btnShowGraph_clicked(bool checked)
{
	QList<QString> lines;	// �������� ������ �� ����������� TextEdit.

	ui.btnNext->setEnabled(false);
	ui.btnPrevious->setEnabled(false);
	ui.labGraphImage->clear();

	// ���� ������� ������ � ������� - �������.
	if (!validateFormat(&lines, NULL))
	{
		QMessageBox::warning(NULL, QString("������"), QString("������ ��� �� ������������� ������� x y w."));
		return;
	}
	if (lines.size() == 0)
		return;

	// ���������� dot-����.
	QString tmpFileName = appPath + QString("tmp.dot");
	FILE * file;
	if (fopen_s(&file, tmpFileName.toLocal8Bit().data(), "w"))
		return;
	fprintf_s(file, "digraph {\nrankdir = LR;\n");
	for (QList<QString>::const_iterator iter = lines.constBegin(); iter != lines.constEnd(); iter++)
	{
		QRegExp regex("\\s*(\\d+)\\s+(\\d+)\\s+(-?\\d+)\\s*");
		regex.exactMatch(*iter);
		fprintf_s(file, "%d -> %d [label=\"%d\"];\n", regex.cap(1).toInt(), regex.cap(2).toInt(), regex.cap(3).toInt());
	}
	fprintf_s(file, "};");
	fclose(file);

	QStringList args;
	args << QString("-Tpng") << QString("-o") + tmpFileName + QString(".png") << tmpFileName;
	QProcess::execute(dotExeFileName, args);
	ui.labGraphImage->setPixmap(tmpFileName + QString(".png"));
	_unlink(tmpFileName.toLocal8Bit().data());
	_unlink((tmpFileName + QString(".png")).toLocal8Bit().data());
	cleanUp();
}

void GUI::btnSearch_clicked(bool checked)
{
	QList<QString> lines;	// �������� ������ �� ����������� TextEdit.
	int maxVertex = -1;		// ������������ ������������� ����� �������.

	ui.btnNext->setEnabled(false);
	ui.btnPrevious->setEnabled(false);
	ui.labGraphImage->clear();

	// ���� ������� ������ � ������� - �������.
	if (!validateFormat(&lines, &maxVertex))
	{
		QMessageBox::warning(NULL, QString("������"), QString("������ ��� �� ������������� ������� x y w."));
		return;
	}
	if (lines.size() == 0)
		return;
	if (ui.leStartVertex->text() == ui.leEndVertex->text())
	{
		QMessageBox::warning(NULL, QString("������"), QString("��������� � �������� ������� �������� ���������."));
		return;
	}

	cleanUp();

	// ��������� �������� ����� ��� ����������� ���������� � ����������.
	QString inputFileName = appPath + QString("in.txt");
	FILE * file;
	if (fopen_s(&file, inputFileName.toLocal8Bit().data(), "w") != 0)
	{
		QMessageBox::warning(NULL, QString("������"), QString("�� ������� ������������� ������� ���� ��� ������ ���������."));
		return;
	}
	fprintf_s(file, "%d\n %d %d\n", (int)lines.size(), ui.leStartVertex->text().toInt(), ui.leEndVertex->text().toInt());
	for (QList<QString>::const_iterator iter = lines.constBegin(); iter != lines.constEnd(); iter++)
		fprintf_s(file, "%s\n", iter->toLocal8Bit().data());
	fclose(file);

	// ������ ���������.
	QString daExeFileName = appPath + QString("DijkstrasAlgorithm.exe");
	QString outputFileName = appPath + QString("out.txt");
	QString prefixFileName = appPath + QString("step");
	QStringList args;
	args << inputFileName << outputFileName << prefixFileName;
	QProcess::execute(daExeFileName, args);
	_unlink(inputFileName.toLocal8Bit().data());

	// ���������� ������ ��� ������������.
	statusBar()->showMessage(QString("���������� ���������..."));
	if (fopen_s(&file, outputFileName.toLocal8Bit().data(), "r") != 0)
	{
		statusBar()->showMessage(QString("�� ������� ������� ���� � ������������ ������ ���������."));
		return;
	}
	char buf[256] = "";
	int linesCount = 0;
	fgets(buf, 256, file);
	fscanf_s(file, "%d\n", &linesCount);
	buf[strlen(buf) - 1] = '\0';
	if (strcmp(buf, "fail") == 0)
	{
		// ������� ��������� �� �������.
		QString errors;
		for (int i = 0; i < linesCount; i++)
		{
			fgets(buf, 256, file);
			errors += QString(buf);
			if (i != linesCount - 1)
				errors += QString("\n");
		}
		statusBar()->showMessage(QString(""));
		QMessageBox::warning(NULL, QString("������ �� ������� ������."), errors);
	}
	else
	{
		int pathSize = 0;
		fscanf_s(file, "%d\n", &pathSize);
		// ������������� ���� ���������.
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
		if (pathSize == 0)
			statusBar()->showMessage(QString("���� �� ������."));
		else
			statusBar()->showMessage(QString("���� ������"));
		// ���������� ��������� ���.
		ui.labGraphImage->setPixmap(QPixmap(images[0]));
		ui.btnNext->setEnabled(true);
		ui.btnPrevious->setEnabled(false);
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
	ui.btnNext->setEnabled(true);
	ui.btnPrevious->setEnabled(currentImage > 0);
}

void GUI::btnNext_clicked(bool checked)
{
	if (images.empty())
		return;
	currentImage++;
	if (currentImage == images.size())
		currentImage = 0;
	ui.labGraphImage->setPixmap(QPixmap(images[currentImage]));
	ui.btnNext->setEnabled(currentImage < images.size() - 1);
	ui.btnPrevious->setEnabled(true);
}

void GUI::btnMenuOpen_triggered(bool checked)
{
	QString fileName = QFileDialog::getOpenFileName(this, QString("������� ����"), QString(""), QString("���� �������� ����� (*.graph)"), 0, 0);
	if (fileName == QString(""))
		return;
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(NULL, QString("������"), QString("�� ������� ������� ����."));
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
	QString fileName = QFileDialog::getSaveFileName(this, QString("��������� ����"), QString(""), QString("���� �������� ����� (*.graph)"), 0, 0);
	if (fileName == QString(""))
		return;
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(NULL, QString("������"), QString("�� ������� ��������� ����."));
		return;
	}
	QTextStream stream(&file);
	QTextDocument * doc = ui.teGraph->document();	// ���������� TextEdit.
	QList<QString> lines;							// �������� ������ �� ����������� TextEdit.
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
	QMessageBox::information(NULL, QString("������������� ���������"),	QString("���� �������������� ������� ���, ������� ������ ����� ������ <�������> <�������> <���>\n") +
																		QString("��������, ������ ��� ����� ����� ���:\n0 1 3\n1 2 2\n\n") +
																		QString("�� ������� �� ������ \"�������� ����\" �������� ����������� ������������� ����� � ������� ������ ������ ���.\n") +
																		QString("�� ������� �� ������ \"����� ���������� ����\" ����� �������� �������� ��������, ���� �������� ����� ����������� � ����������� ����.\n") +
																		QString("������ \"���������� ���\" � \"��������� ���\" ��������� ������������ ���� ���������.\n\n") +
																		QString("������� ����� �������������, ��� ������ ������� �������� ����� len, ������ ����� ���� �� ��������� ������� �� ���. ����� len=-1 �������� ����������� ����� ����. ") +
																		QString("������� ��������������� ���� ���������� ������� ������, ������������� ������� ���������� ���������, � ��������� �� ��� ���� - ����� ������. ") +
																		QString("�� ��������� ���� ����, ������������� ���������� ����, ���������� ���������� ������.\n"));
}

void GUI::btnMenuAlgorithm_triggered(bool checked)
{
	QMessageBox::information(NULL, QString("�� ���������"),	QString("�������� �������� � �������� �� ������, ������������ ������������� ������ �. ��������� � 1959 ����. ������� ���������� ���������� �� ����� �� ������ ����� �� ") +
															QString("���� ���������. �������� �������� ������ ��� ������ ��� ��� � ������������� ����� � ������.\n\n") +
															QString("�������������: ����� ��������� ������� �������� ���������� ������ 0, ����� ��������� ������ � �������������. ��� �������� ��, ��� ���������� �� ��������� ������� �� ������ ������ ���� ����������. ��� ������� ����� ���������� ��� ������������.\n\n") +
															QString("��� ���������: ���� ��� ������� ��������, �������� �����������. � ��������� ������, �� ��� �� ���������� ������ ���������� ������� u, ������� ����������� �����. �������, � ������� ����� ����� �� u, ������� �������� ���� �������. ") +
															QString("��� ������� ������ ������� u, ����� ���������� ��� ����������, ���������� ����� ����� ����, ������ ����� �������� ������� ����� u � ����� �����, ������������ u � ���� �������. ") +
															QString("���� ���������� �������� ����� ������ �������� ����� ������, ������� �������� ����� ���������� ��������� �����. ���������� ���� �������, ������� ������� u ��� ���������� � �������� ��� ���������."));
}

void GUI::btnMenuAbout_triggered(bool checked)
{
	QMessageBox::information(NULL, QString("� ���������"), QString("<p align='center'>������������ ������ ��������� ��������.<br><br>������: �������� ������ ���-360 ��������� �. �., ��������� �. �.<br><br>������������ ��������������� ����������� �����������</center><br>������� ����<br>2012 ���</p>"));
}