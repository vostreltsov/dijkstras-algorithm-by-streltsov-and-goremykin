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
		ExecutionState res = G.run("C:\\step", NULL);
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
		ExecutionState res = G.run("C:\\step", NULL);

		assertTrue(res.totalWeight == 10, "Неверная длина пути (тест № 1)");
		assertTrue(res.path.size() == 1, "Неверное количество переходов (тест № 1)");
		assertTrue(res.path[0].weight == 10, "Неправильный переход (тест № 1)");		
	}

	// Тест из описания алгоритма на Википедии.
	void test2()
	{
		Graph G;
		std::vector<Edge> edges;
		edges.push_back(Edge((Node *)0, (Node *)1, 7));
		edges.push_back(Edge((Node *)0, (Node *)2, 9));
		edges.push_back(Edge((Node *)0, (Node *)5, 14));
		edges.push_back(Edge((Node *)1, (Node *)2, 10));
		edges.push_back(Edge((Node *)1, (Node *)3, 15));
		edges.push_back(Edge((Node *)2, (Node *)3, 11));
		edges.push_back(Edge((Node *)2, (Node *)5, 2));
		edges.push_back(Edge((Node *)3, (Node *)4, 6));
		edges.push_back(Edge((Node *)5, (Node *)4, 9));

		G.build(6, edges);
		G.startNode = &G.nodes[0];
		G.endNode = &G.nodes[5];
		ExecutionState res = G.run("C:\\step", NULL);
		assertTrue(res.totalWeight == 11, "Неверная длина пути (тест № 2)");
		assertTrue(res.path.size() == 2, "Неверное количество переходов (тест № 2)");
		assertTrue(res.path[0].weight == 9 && res.path[1].weight == 2, "Найдены неправильные переходы (тест № 2)");		

		G.startNode = &G.nodes[0];
		G.endNode = &G.nodes[3];
		res = G.run("C:\\step", NULL);
		assertTrue(res.totalWeight == 20, "Неверная длина пути (тест № 2)");
		assertTrue(res.path.size() == 2, "Неверное количество переходов (тест № 2)");
		assertTrue(res.path[0].weight == 9 && res.path[1].weight == 11, "Найдены неправильные переходы (тест № 2)");	
	}

	// Путь до одной из вершин напрямую длинее, чем в обход.
	void test3()
	{
		Graph G;
		std::vector<Edge> edges;
		edges.push_back(Edge((Node *)0, (Node *)1, 10));
		edges.push_back(Edge((Node *)0, (Node *)2, 1));
		edges.push_back(Edge((Node *)1, (Node *)3, 5));
		edges.push_back(Edge((Node *)2, (Node *)1, 2));
		edges.push_back(Edge((Node *)2, (Node *)3, 20));

		G.build(4, edges);
		G.startNode = &G.nodes[0];
		G.endNode = &G.nodes[3];
		ExecutionState res = G.run("C:\\step", NULL);
		assertTrue(res.totalWeight == 8, "Неверная длина пути (тест № 3)");
		assertTrue(res.path.size() == 3, "Неверное количество переходов (тест № 3)");
		assertTrue(res.path[0].weight == 1 && res.path[1].weight == 2 && res.path[2].weight == 5, "Найдены неправильные переходы (тест № 3)");			
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