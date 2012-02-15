#include <stdio.h>
#include <conio.h>
#include "graph.h"

#ifdef _DEBUG
	// TODO ���������� ������� ������������
#endif

int main(int argc, char *argv[])
{

#ifdef _DEBUG
	// TODO ��������� ����-�����
	Graph G("D:\\1.txt");
	if (G.error_exists())
	{
		std::vector<int> errors = G.getErrors();
		for (int i = 0; i < errors.size(); i++)
		{
			printf("%s\n", Graph::getErrorString(errors[i]));
		}
	}
	_getch();
#else
	// TODO ������� ����, ��������� �������� � ������ ���������.
#endif

	return 0;
}