#include <stdio.h>
#include <conio.h>
#include "graph.h"

#ifdef _DEBUG
	#include "testing.h"
#endif

int main(int argc, char *argv[])
{

#ifdef _DEBUG
	TestSuite tests;
	tests.run();
	_getch();
#else
	// TODO считать файл, запустить алгоритм и выдать результат.
	/*Graph G("D:\\1.txt");
	if (G.error_exists())
	{
		std::vector<int> errors = G.getErrors();
		for (int i = 0; i < errors.size(); i++)
		{
			printf("%s\n", Graph::getErrorString(errors[i]));
		}
	}*/
#endif

	return 0;
}