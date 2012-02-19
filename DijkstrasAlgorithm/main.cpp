#define _CRTDBG_MAP_ALLOC

#include <stdio.h>
#include <conio.h>
#include <locale>
#include <crtdbg.h>
#include "graph.h"

#ifdef _DEBUG
	#include "testing.h"
#endif

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "rus");

#ifdef _DEBUG
	TestSuite tests;
	tests.run();
	_getch();
	_CrtDumpMemoryLeaks();
	return 0;
#else
	if (argc < 4)
	{
		printf("Too few command line arguments. Example usage: qwe.exe \"C:\\in.txt\" \"C:\\out.txt\" \"C:\\dotfileprefix\".\n");
		return 0;
	}
	Graph * G = new Graph(argv[1]);
	if (G->error_exists())
	{
		printf("Errors found:\n");
		std::vector<int> errors = G->getErrors();
		for (int i = 0; i < errors.size(); i++)
			printf("%s\n", Graph::getErrorString(errors[i]));
		return 0;
	}
	std::vector<char *> * dotFilesGenerated = new std::vector<char *>;
	G->run(argv[3], dotFilesGenerated);

	// Формируем выходной файл.
	FILE * file;
	if (!fopen_s(&file, argv[2], "w"))
	{
		for (int i = 0; i < dotFilesGenerated->size(); i++)
			fprintf_s(file, "%s\n", (*dotFilesGenerated)[i]);
		fclose(file);
	} else
		printf("Could not create output file.");

	// Очищаем память.
	delete G;
	for (int i = 0; i < dotFilesGenerated->size(); i++)
		delete (*dotFilesGenerated)[i];
	delete dotFilesGenerated;
	return 0;
#endif
}