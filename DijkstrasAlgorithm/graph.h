#pragma once
#include <stdio.h>
#include <vector>

struct Transition
{
	int from;
	int to;
	int weight;
};

struct Node
{
	int number;
	int tag;
};

class Graph
{
private:
	std::vector<Node> nodes;
};