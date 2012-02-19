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

bool Edge::operator==(const Edge & other) const
{
	return (from == other.from && to == other.to && weight == other.weight);
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
	startNode = NULL;
	endNode = NULL;
}

Graph::Graph(const char * fileName)
{
	readFromFile(fileName);	// ��������� ���������� ������ ���� �������.
}

void Graph::build(const int vertexCount, std::vector<Edge> edges)
{
	nodes.clear();
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

void Graph::validate(const int vertexCount, std::vector<Edge> edges, const int start, const int end)
{
	bool negativeWeight = false;
	bool loopExists = false;	
	bool unknownNode = false;
	bool unknownBorders = (start < 0 || start >= vertexCount || end < 0 || end >= vertexCount);

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
	if (unknownBorders)
		errors.push_back(Graph::ERROR_PATH_BORDERS_NOT_EXIST);
}

bool Graph::readFromFile(const char * fileName)
{
	int n = 0;			// ����� ������ � �����.
	int pathStart = 0;	// ��������� ������� ��������.
	int pathEnd = 0;	// �������� ������� ��������.

	FILE * file;
	if (fopen_s(&file, fileName, "r"))
	{
		errors.clear();
		errors.push_back(Graph::ERROR_COULD_NOT_OPEN_FILE);
		return false;
	}
	
	// ������ ���������� �����, ������ ���������� � ��������� ����� ��������.
	fscanf_s(file, "%d", &n);
	fscanf_s(file, "%d", &pathStart);
	fscanf_s(file, "%d", &pathEnd);

	// ���������� ����� ����� - ���������� � �����.
	std::vector<Edge> edges;
	while (!feof(file))
	{
		int edgeStart = 0;
		int edgeEnd = 0;
		int edgeWeight = 0;
		fscanf_s(file, "%d %d %d", &edgeStart, &edgeEnd, &edgeWeight);
		edges.push_back(Edge((Node *)edgeStart, (Node *)edgeEnd, edgeWeight));	// ��� ��������� ������ ���������� - �������.
	}
	fclose(file);

	// ��������� ��������� ������ � ������ ����, ���� ��� ���������.
	validate(n, edges, pathStart, pathEnd);
	if (errors.empty())
	{
		build(n, edges);
		startNode = &nodes[pathStart];
		endNode = &nodes[pathEnd];
	}
	else
	{
		startNode = NULL;
		endNode = NULL;
	}
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
	case ERROR_PATH_BORDERS_NOT_EXIST:
		return "Start or end node of the path does not exist in the graph";
	case ERROR_COULD_NOT_OPEN_FILE:
		return "Could not open file";
	default:
		return "Unknown error";
	}
}

ExecutionState Graph::run(const char * fileNamePrefix, std::vector<char *> * dotFilesGenerated)
{
	int stepCount = 0;
	char dotFileName[256] = "";

	// ������� ������ ExecutionState ��� ������� ���� ����� � ���������� ��������� ���������.
	std::vector<ExecutionState *> states;
	ExecutionState * startState = NULL;
	for (int i = 0; i < nodes.size(); i++)
	{
		ExecutionState * newState = new ExecutionState(&nodes[i]);
		if (newState->node == startNode)
			startState = newState;
		states.push_back(newState);
	}
	startState->totalWeight = 0;
	// ���������� ���� � ������ ���������� ���������.
	sprintf_s(dotFileName, 256, "%s%d.dot", fileNamePrefix, stepCount++);
	if (dotFilesGenerated != NULL)
	{
		char * tmp = new char[256];
		strcpy_s(tmp, 256, dotFileName);
		dotFilesGenerated->push_back(tmp);
	}
	generateDotCode(dotFileName, &states, Edge());

	// ��������� ��������.
	ExecutionState * currentState = startState;	// ������� � ����������� ������.
	while (currentState != NULL)
	{
		// ������������� ���� ������� ������� �������.
		for (int i = 0; i < currentState->node->edges.size(); i++)
		{
			Edge edge = currentState->node->edges[i];
			ExecutionState * destState = states[edge.to->number];	// C��������, ��������������� �������� ������� �����.

			// �������������� ���� �� �������� ������� ������� ����.
			if (destState->totalWeight == -1 || destState->totalWeight > currentState->totalWeight + edge.weight)
			{
				destState->path = currentState->path;
				destState->path.push_back(edge);
				destState->totalWeight = currentState->totalWeight + edge.weight;
			}

			// ���������� ���� � �������� ���������� ���������.
			sprintf_s(dotFileName, 256, "%s%d.dot", fileNamePrefix, stepCount++);
			if (dotFilesGenerated != NULL)
			{
				char * tmp = new char[256];
				strcpy_s(tmp, 256, dotFileName);
				dotFilesGenerated->push_back(tmp);
			}
			generateDotCode(dotFileName, &states, edge);

		}		
		// �������� ������� ��� ���������� � �������� ����� ������� � ����������� ������.
		currentState->passed = true;
		currentState = NULL;
		for (int i = 0; i < states.size(); i++)
		{
			ExecutionState * tmp = states[i];
			if (!tmp->passed)
			{
				// �������� ������� �������, ���� ��� ���� ��� ����� NULL ��� ������� ������� � ������� ������.
				if (currentState == NULL || (tmp->totalWeight != -1 && currentState->totalWeight > tmp->totalWeight))
					currentState = tmp;
			}
		}
	}

	// ���������� ���� � ����� ���������� ���������.
	sprintf_s(dotFileName, 256, "%s%d.dot", fileNamePrefix, stepCount++);
	if (dotFilesGenerated != NULL)
	{
		char * tmp = new char[256];
		strcpy_s(tmp, 256, dotFileName);
		dotFilesGenerated->push_back(tmp);
	}
	generateDotCode(dotFileName, &states, Edge());

	// ��������� ��������� � ������� ���������� ������.
	ExecutionState result;
	for (int i = 0; i < states.size(); i++)
	{
		if (states[i]->node == endNode)
			result = *states[i];
		delete states[i];
	}
	return result;
}

bool Graph::generateDotCode(const char * fileName, const std::vector<ExecutionState *> * states, const Edge currentEdge)
{
	FILE * file;
	if (fopen_s(&file, fileName, "w"))
		return false;

	fprintf_s(file, "digraph {\nrankdir = LR;\n");
	// ������ ����.
	std::vector<char *> nodestrings;
	for (int i = 0; i < states->size(); i++)
	{
		ExecutionState * state = (*states)[i];
		char * str = new char[256];
		sprintf_s(str, 256, "\"%d\\n len=%d\"", state->node->number, state->totalWeight);
		
		nodestrings.push_back(str);
		
		char tmp[256] = "";
		strcpy_s(tmp, 256, str);
		if (state->passed)
			strcat_s(tmp, 256, "[style=dotted]");	// �������� ���������� ��������� ���������.
		strcat_s(tmp, 256, ";");
		
		fprintf_s(file, "%s\n", tmp);
	}
	// ������ ��������.
	for (int i = 0; i < states->size(); i++)
	{
		ExecutionState * state = (*states)[i];
		std::vector<Edge> edges = state->node->edges;
		for (int j = 0; j < edges.size(); j++)
		{
			char from[64] = "";
			char to[64] = "";
			char str[256] = "";
			strcpy_s(from, 64, nodestrings[edges[j].from->number]);
			strcpy_s(to, 64, nodestrings[edges[j].to->number]);
			sprintf_s(str, 256, "%s -> %s", from, to);

			char extra[256] = "";
			if (edges[j] == currentEdge)
				sprintf_s(extra, 256, "[label=\"%d\", color=red];", edges[j].weight);	// �������� ������� ���� ������� ������.
			else if ((*states)[edges[j].from->number]->passed)
				sprintf_s(extra, 256, "[label=\"%d\", color=blue];", edges[j].weight);	// �������� ���������� ���� ����� ������.
			else
				sprintf_s(extra, 256, "[label=\"%d\"];", edges[j].weight);
			strcat_s(str, 256, extra);
			fprintf_s(file, "%s\n", str);
		}
	}
	fprintf_s(file, "};");

	fclose(file);
	for (int i = 0; i < nodestrings.size(); i++)
		delete nodestrings[i];
	return true;
}
/*----------------------------------------------------------------------------------------------------*/

ExecutionState::ExecutionState()
{
	node = NULL;
	totalWeight = -1;
	passed = false;
}

ExecutionState::ExecutionState(const Node * _node)
{
	node = const_cast<Node *>(_node);
	totalWeight = -1;
	passed = false;
}