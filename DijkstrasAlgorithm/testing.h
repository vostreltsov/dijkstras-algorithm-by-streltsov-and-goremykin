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

	void test1()
	{
		Graph G;
		std::vector<Edge> edges;
		edges.push_back(Edge((Node *)0, (Node *)1, 10));
		edges.push_back(Edge((Node *)1, (Node *)0, 15));
		G.build(2, edges);
		G.startNode = &G.nodes[0];
		G.endNode = &G.nodes[1];
		assertTrue(1 == 0, "Mathematics is powerless");
	}

	void run()
	{
		test1();
		printf("\nTesting complete: %d passes and %d fails.", passCount, failCount);
	}
};