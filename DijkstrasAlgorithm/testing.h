#pragma once
#include <stdio.h>
#include "graph.h"

class TestSuite
{
private:
	int passCount;	// Количество пройденных тестов.
	int failCount;	// Количество проваленных тестов.

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

	// Вырожденный случай - граф из единственной вершины.
	void test0()
	{
		Graph G;
		std::vector<Edge> edges;
		G.build(1, edges);
		G.startNode = &G.nodes[0];
		G.endNode = &G.nodes[0];
		ExecutionState res = G.run();
		assertTrue(res.totalWeight == 0, "Неверная длина пути (тест № 0)");
		assertTrue(res.path.empty(), "Неверное количество переходов (тест № 0)");
	}

	// Граф из двух вершин, из первой идет 2 дуги во вторую с разной длиной
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
		assertTrue(res.totalWeight == 10, "Неверная длина пути (тест № 1)");
		assertTrue(res.path.size() == 1, "Неверное количество переходов (тест № 1)");
		assertTrue(res.path[0].weight == 10, "Неправильный переход (тест № 1)");		
	}

	void run()
	{
		test0();
		test1();
		printf("\nTesting complete: %d passes and %d fails.", passCount, failCount);
	}
};