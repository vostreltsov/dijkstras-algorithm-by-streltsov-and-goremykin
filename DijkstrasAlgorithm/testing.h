#pragma once
#include <stdio.h>
#include "graph.h"

class TestSuite
{
private:
	int passCount;	// ���������� ���������� ������.
	int failCount;	// ���������� ����������� ������.

	void assertTrue(const bool condition, const char * failMessage = "")
	{
		if (condition)
			passCount++;
		else
		{
			failCount++;
			printf("FAIL: %s\n", failMessage);
		}
	}

	void cleanUp(std::vector<std::string> dotFilesGenerated)
	{
		for (std::vector<std::string>::const_iterator iter = dotFilesGenerated.cbegin(); iter != dotFilesGenerated.cend(); iter++)
			_unlink(iter->c_str());
	}

public:
	TestSuite()
	{
		passCount = 0;
		failCount = 0;
	}

	// ����������� ������ - ���� �� ������������ �������.
	void test0()
	{
		Graph G;
		std::vector<FileListItem> edges;
		std::vector<std::string> dotFilesGenerated;

		edges.push_back(FileListItem("0", "0", 10));

		G.build(edges);
		G.startNode = G.nodes.find("0")->second;
		G.endNode = G.nodes.find("0")->second;
		ExecutionState res = G.run("C:\\step", &dotFilesGenerated);
		cleanUp(dotFilesGenerated);

		assertTrue(res.totalWeight == -1, "�������� ����� ���� (���� � 0)");
		assertTrue(res.path.empty(), "�������� ���������� ��������� (���� � 0)");
	}

	// ���� �� ���� ������, �� ������ ���� 2 ���� �� ������ � ������ ������
	void test1()
	{
		Graph G;
		std::vector<FileListItem> edges;
		std::vector<std::string> dotFilesGenerated;

		edges.push_back(FileListItem("0", "1", 15));
		edges.push_back(FileListItem("0", "1", 10));


		G.build(edges);
		G.startNode = G.nodes.find("0")->second;
		G.endNode = G.nodes.find("1")->second;
		ExecutionState res = G.run("C:\\step", &dotFilesGenerated);
		cleanUp(dotFilesGenerated);

		assertTrue(res.totalWeight == 10, "�������� ����� ���� (���� � 1)");
		assertTrue(res.path.size() == 1, "�������� ���������� ��������� (���� � 1)");
		assertTrue(res.path[0]->weight == 10, "������������ ������� (���� � 1)");
	}

	// ���� �� �������� ��������� �� ���������.
	void test2()
	{
		Graph G;
		std::vector<FileListItem> edges;
		std::vector<std::string> dotFilesGenerated;

		edges.push_back(FileListItem("0", "1", 7));
		edges.push_back(FileListItem("0", "2", 9));
		edges.push_back(FileListItem("0", "5", 14));
		edges.push_back(FileListItem("1", "2", 10));
		edges.push_back(FileListItem("1", "3", 15));
		edges.push_back(FileListItem("2", "3", 11));
		edges.push_back(FileListItem("2", "5", 2));
		edges.push_back(FileListItem("3", "4", 6));
		edges.push_back(FileListItem("4", "4", 9));

		G.build(edges);
		G.startNode = G.nodes.find("0")->second;
		G.endNode = G.nodes.find("5")->second;
		ExecutionState res = G.run("C:\\step", &dotFilesGenerated);
		cleanUp(dotFilesGenerated);

		assertTrue(res.totalWeight == 11, "�������� ����� ���� (���� � 2)");
		assertTrue(res.path.size() == 2, "�������� ���������� ��������� (���� � 2)");
		assertTrue(res.path[0]->weight == 9 && res.path[1]->weight == 2, "������� ������������ �������� (���� � 2)");

		G.startNode = G.nodes.find("0")->second;
		G.endNode = G.nodes.find("3")->second;
		res = G.run("C:\\step", &dotFilesGenerated);
		cleanUp(dotFilesGenerated);

		assertTrue(res.totalWeight == 20, "�������� ����� ���� (���� � 2)");
		assertTrue(res.path.size() == 2, "�������� ���������� ��������� (���� � 2)");
		assertTrue(res.path[0]->weight == 9 && res.path[1]->weight == 11, "������� ������������ �������� (���� � 2)");

		G.startNode = G.nodes.find("5")->second;
		G.endNode = G.nodes.find("1")->second;
		res = G.run("C:\\step", &dotFilesGenerated);
		cleanUp(dotFilesGenerated);

		assertTrue(res.totalWeight == -1, "�������� ����� ���� (���� � 2)");
		assertTrue(res.path.empty(), "�������� ���������� ��������� (���� � 2)");
	}

	// ���� �� ����� �� ������ �������� ������, ��� � �����.
	void test3()
	{
		Graph G;
		std::vector<FileListItem> edges;
		std::vector<std::string> dotFilesGenerated;

		edges.push_back(FileListItem("0", "1", 10));
		edges.push_back(FileListItem("0", "2", 1));
		edges.push_back(FileListItem("1", "3", 5));
		edges.push_back(FileListItem("2", "1", 2));
		edges.push_back(FileListItem("2", "3", 20));

		G.build(edges);
		G.startNode = G.nodes.find("0")->second;
		G.endNode = G.nodes.find("3")->second;
		ExecutionState res = G.run("C:\\step", &dotFilesGenerated);
		cleanUp(dotFilesGenerated);

		assertTrue(res.totalWeight == 8, "�������� ����� ���� (���� � 3)");
		assertTrue(res.path.size() == 3, "�������� ���������� ��������� (���� � 3)");
		assertTrue(res.path[0]->weight == 1 && res.path[1]->weight == 2 && res.path[2]->weight == 5, "������� ������������ �������� (���� � 3)");
	}

	void run()
	{
		test0();
		test1();
		test2();
		test3();
		printf("\nTesting complete: %d passes and %d fails.", passCount, failCount);
	}
};