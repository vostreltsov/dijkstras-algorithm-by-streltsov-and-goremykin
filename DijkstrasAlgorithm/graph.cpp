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

void Graph::validate()
{
	errors.clear();
	// TODO ����� �����, ���� � ������������� ����� � �.�.
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

	// ��������� n ����� � ����.
	for (int i = 0; i < n; i++)
		nodes.push_back(Node(i));

	// ���������� ����� ����� - ���������� � �����.
	while (!feof(file))
	{
		int edgeStart = 0;
		int edgeEnd = 0;
		int edgeWeight = 0;
		fscanf(file, "%d %d %d", &edgeStart, &edgeEnd, &edgeWeight);
		nodes[edgeStart].edges.push_back(Edge(&nodes[edgeStart], &nodes[edgeEnd], edgeWeight));
	}
	fclose(file);

	validate();
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

std::vector<Edge> Graph::run(const int start, const int end)
{
	return std::vector<Edge>();
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