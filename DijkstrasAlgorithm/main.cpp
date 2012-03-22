#define _CRTDBG_MAP_ALLOC

#include <stdio.h>
#include <conio.h>
#include <locale>
#include <crtdbg.h>
#include <string>
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
	FILE * file;
	Graph * G = new Graph(argv[1]);
	if (!fopen_s(&file, argv[2], "w"))
	{
		if (G->error_exists())
		{
			// Пишем в файл найденные ошибки.
			printf("Errors found:\n");
			std::vector<int> errors = G->getErrors();
			fprintf_s(file, "fail\n%d\n", (int)errors.size());
			for (int i = 0; i < errors.size(); i++)
			{
				printf("%s\n", Graph::getErrorString(errors[i]));
				fprintf_s(file, "%s\n", Graph::getErrorString(errors[i]));
			}
		}
		else
		{
			// Пишем в файл результаты работы алгоритма.
			std::vector<std::string> * dotFilesGenerated = new std::vector<std::string>;
			G->run(argv[3], dotFilesGenerated);
			fprintf_s(file, "success\n%d\n", (int)dotFilesGenerated->size());
			for (int i = 0; i < dotFilesGenerated->size(); i++)
				fprintf_s(file, "%s\n", (*dotFilesGenerated)[i]);
			delete dotFilesGenerated;
		}
		fclose(file);
	} else
		printf("Could not create output file.");

	delete G;
	return 0;
#endif
}