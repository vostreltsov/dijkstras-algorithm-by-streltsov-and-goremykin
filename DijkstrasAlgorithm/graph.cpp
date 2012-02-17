#include "graph.h"

Edge::Edge(Node * _from, Node * _to, const int _weight)
{
	from = _from;
	to = _to;
	weight = _weight;
}

Edge::Edge()
{
	from = NULL;
	to = NULL;
	weight = 0;
}

/*----------------------------------------------------------------------------------------------------*/

Node::Node()
{
	number = -1;	// ��������� ����� � ����� ���� � ����.
}

Node::Node(const int _number)
{
	number = _number;
}

/*----------------------------------------------------------------------------------------------------*/

Graph::Graph()
{
}

Graph::Graph(const char * fileName)
{
	readFromFile(fileName);	// ��������� ���������� ������ ���� �������.
}

void Graph::build(const int vertexCount, std::vector<Edge> edges)
{
	// ��������� vertexCount ����� � ����.
	for (int i = 0; i < vertexCount; i++)
		nodes.push_back(Node(i));
	// ������ ����� ����� ������.
	for (int i = 0; i < edges.size(); i++)
	{
		Node * from = &nodes[(int)edges[i].from];
		Node * to = &nodes[(int)edges[i].to];
		nodes[from->number].edges.push_back(Edge(from, to, edges[i].weight));
	}
}

void Graph::validate(const int vertexCount, std::vector<Edge> edges)
{
	bool negativeWeight = false;
	bool loopExists = false;	
	bool unknownNode = false;

	for (int i = 0; i < edges.size(); i++)
	{
		// ���� �� ���� � ������������� �����?
		if (edges[i].weight < 0)
			negativeWeight = true;
		// ���� �� �����?
		if (edges[i].from == edges[i].to)
			loopExists = true;
		// �� ������� �� �������������� �������?
		if ((int)edges[i].from < 0 || (int)edges[i].from >= vertexCount || (int)edges[i].to < 0 || (int)edges[i].to >= vertexCount)
			unknownNode = true;
	}

	// ��������� ������ ������.
	errors.clear();
	if (negativeWeight)
		errors.push_back(Graph::ERROR_NEGATIVE_WEIGHT);
	if (loopExists)
		errors.push_back(Graph::ERROR_LOOP_EXISTS);
	if (unknownNode)
		errors.push_back(Graph::ERROR_UNKNOWN_NODE);
}

bool Graph::readFromFile(const char * fileName)
{
	int n = 0;			// ����� ������ � �����.
	int pathStart = 0;	// ��������� ������� ��������.
	int pathEnd = 0;	// �������� ������� ��������.

	FILE * file = fopen(fileName, "r");
	if (file == NULL)
		return false;
	
	// ������ ���������� �����, ������ ���������� � ��������� ����� ��������.
	fscanf(file, "%d", &n);
	fscanf(file, "%d", &pathStart);
	fscanf(file, "%d", &pathEnd);

	// ���������� ����� ����� - ���������� � �����.
	std::vector<Edge> edges;
	while (!feof(file))
	{
		int edgeStart = 0;
		int edgeEnd = 0;
		int edgeWeight = 0;
		fscanf(file, "%d %d %d", &edgeStart, &edgeEnd, &edgeWeight);
		edges.push_back(Edge((Node *)edgeStart, (Node *)edgeEnd, edgeWeight));	// ��� ��������� ������ ���������� - �������.
	}
	fclose(file);

	// ��������� ��������� ������ � ������ ����, ���� ��� ���������.
	validate(n, edges);
	nodes.clear();
	if (errors.empty())
		build(n, edges);
	return true;
}

bool Graph::error_exists()
{
	return !errors.empty();
}

std::vector<int> Graph::getErrors()
{
	return errors;
}

char * Graph::getErrorString(const int errorCode)
{
	switch (errorCode)
	{
	case ERROR_NOT_EXISTS:
		return "No errors were found";
	case ERROR_NEGATIVE_WEIGHT:
		return "An edge with negative weight was found";
	case ERROR_LOOP_EXISTS:
		return "A loop was found";
	case ERROR_UNKNOWN_NODE:
		return "A transition to (from) an undefined vertex was found";
	default:
		return "Unknown error";
	}
}

std::vector<Edge> Graph::run(const int start, const int end)
{
	return std::vector<Edge>();
}

bool generateDotCode(const char * fileName, const std::vector<ExecutionState> * states, const Edge * currentEdge)
{
	return true;
}
/*----------------------------------------------------------------------------------------------------*/

ExecutionState::ExecutionState()
{
	node = NULL;
}

ExecutionState::ExecutionState(const Node * _node)
{
	node = const_cast<Node *>(_node);
}