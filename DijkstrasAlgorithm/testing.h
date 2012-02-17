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
		std::vector<Edge> edges;
		G.build(1, edges);
		G.startNode = &G.nodes[0];
		G.endNode = &G.nodes[0];
		ExecutionState res = G.run();
		assertTrue(res.totalWeight == 0, "�������� ����� ���� (���� � 0)");
		assertTrue(res.path.empty(), "�������� ���������� ��������� (���� � 0)");
	}

	// ���� �� ���� ������, �� ������ ���� 2 ���� �� ������ � ������ ������
	void test1()
	{
		Graph G;
		std::vector<Edge> edges;
		edges.push_back(Edge((Node *)0, (Node *)1, 15));
		edges.push_back(Edge((Node *)0, (Node *)1, 10));
		G.build(2, edges);
		G.startNode = &G.nodes[0];
		G.endNode = &G.nodes[1];
		ExecutionState res = G.run();
		assertTrue(res.totalWeight == 10, "�������� ����� ���� (���� � 1)");
		assertTrue(res.path.size() == 1, "�������� ���������� ��������� (���� � 1)");
		assertTrue(res.path[0].weight == 10, "������������ ������� (���� � 1)");		
	}

	void run()
	{
		test0();
		test1();
		printf("\nTesting complete: %d passes and %d fails.", passCount, failCount);
	}
};