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
	for (size_t i = 0; i < edges.size(); i++)
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

	for (size_t i = 0; i < edges.size(); i++)
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
	int m = 0;			// ����� ��� � �����.
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

	// ������ ���������� ���, �����, ������ ���������� � ��������� ����� ��������.
	fscanf_s(file, "%d\n %d %d %d\n", &m, &n, &pathStart, &pathEnd);

	// ���������� ����� ����� - ���������� � �����.
	std::vector<Edge> edges;
	for (int i = 0; i < m; i++)
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
		return "������ ���";
	case ERROR_NEGATIVE_WEIGHT:
		return "������� ���� � ������������� �����";
	case ERROR_LOOP_EXISTS:
		return "������� �����";
	case ERROR_UNKNOWN_NODE:
		return "������� ���� � ����� ��������������� ���������";
	case ERROR_PATH_BORDERS_NOT_EXIST:
		return "��������� ��� �������� ������� �� ���������� � �����";
	case ERROR_COULD_NOT_OPEN_FILE:
		return "�� ������� ������� ����";
	default:
		return "����������� ������";
	}
}

ExecutionState Graph::run(const char * fileNamePrefix, std::vector<std::string> * dotFilesGenerated)
{
	int stepCount = 0;						// ������� ��������������� ��������.
	std::vector<ExecutionState *> states;	// ������� ���� � ����� �������� � ������������ ������ ExecutionState.
	ExecutionState * startState = NULL;		// ��������� ��������� ��� ����������.

	// ������� ������ ExecutionState ��� ������� ���� ����� � ���������� ��������� ���������.
	for (size_t i = 0; i < nodes.size(); i++)
	{
		ExecutionState * newState = new ExecutionState(&nodes[i]);
		if (newState->node == startNode)
			startState = newState;
		states.push_back(newState);
	}
	startState->totalWeight = 0;

	// ���������� ���� � ������ ���������� ���������.
	dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, Edge()));

	// ��������� ��������.
	ExecutionState * currentState = startState;	// ������� � ����������� ������.
	while (currentState != NULL)
	{
		// ������������� ���� ������� ������� �������.
		for (size_t i = 0; i < currentState->node->edges.size(); i++)
		{
			Edge edge = currentState->node->edges[i];
			ExecutionState * destState = states[edge.to->number];	// C��������, ��������������� �������� ������� �����.

			// �������������� ���� �� �������� ������� ������� ����.
			if (currentState->totalWeight != -1 && (destState->totalWeight == -1 || destState->totalWeight > currentState->totalWeight + edge.weight))
			{
				destState->path = currentState->path;
				destState->path.push_back(edge);
				destState->totalWeight = currentState->totalWeight + edge.weight;
			}

			// ���������� ���� � �������� ���������� ���������.
			dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, edge));

		}
		// �������� ������� ��� ���������� � �������� ����� ������� � ����������� ������.
		currentState->passed = true;
		currentState = NULL;
		for (size_t i = 0; i < states.size(); i++)
		{
			ExecutionState * tmp = states[i];
			if (!tmp->passed)
			{
				// �������� ������� �������, ���� ��� ���� ��� ����� NULL ��� ������� ������� � ������� ������.
				if (currentState == NULL ||
					(tmp->totalWeight != -1 && currentState->totalWeight > tmp->totalWeight) ||
					(currentState->totalWeight == -1 && tmp->totalWeight != -1))
					currentState = tmp;
			}
		}
		// ���������� ���� ����� ����������� ��������� �������.
		dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, Edge()));
	}

	// ��������� ���������.
	ExecutionState result;
	for (size_t i = 0; i < states.size(); i++)
		if (!states[i]->path.empty() && states[i]->path.front().from == startNode && states[i]->path.back().to == endNode)
			result = *states[i];

	// ���������� ����, � ������� ������������ ����������� ����.
	dotFilesGenerated->push_back(generateDotCodeForResult(fileNamePrefix, &stepCount, &states, &result));

	//  ������� ���������� ������.
	for (size_t i = 0; i < states.size(); i++)
		delete states[i];

	return result;
}

std::string Graph::generateDotCodeForStep(const char * fileNamePrefix, int * stepCount, const std::vector<ExecutionState *> * states, const Edge currentEdge)
{
	char tmp[256];
	char fileName[256];
	FILE * file;
	sprintf_s(fileName, 256, "%s%d.dot", fileNamePrefix, (*stepCount)++);
	if (fopen_s(&file, fileName, "w"))
		return std::string("");

	fprintf_s(file, "digraph {\nrankdir = LR;\n");
	// ������ ����.
	std::vector<std::string> nodestrings;
	for (size_t i = 0; i < states->size(); i++)
	{
		ExecutionState * state = (*states)[i];
		sprintf_s(tmp, 256, "\"%d\\n len=%d\"", state->node->number, state->totalWeight);
		nodestrings.push_back(std::string(tmp));

		// ���������� ���� � ����, ������� ���������� ��������� ���������.
		std::string wr(tmp);
		if (state->passed)
			wr.append("[style=dotted]");
		wr.append(";");
		fprintf_s(file, "%s\n", wr.c_str());
	}
	// ������ ��������.
	for (size_t i = 0; i < states->size(); i++)
	{
		ExecutionState * state = (*states)[i];
		std::vector<Edge> edges = state->node->edges;
		for (size_t j = 0; j < edges.size(); j++)
		{
			std::string wr = nodestrings[edges[j].from->number] + " -> " + nodestrings[edges[j].to->number];

			if (edges[j] == currentEdge)
				sprintf_s(tmp, 256, "[label=\"%d\", color=red];", edges[j].weight);		// �������� ������� ���� ������� ������.
			else if ((*states)[edges[j].from->number]->passed)
				sprintf_s(tmp, 256, "[label=\"%d\", color=blue];", edges[j].weight);	// �������� ���������� ���� ����� ������.
			else
				sprintf_s(tmp, 256, "[label=\"%d\"];", edges[j].weight);				// ��������� ���� ����� �� ��������.

			wr.append(tmp);
			fprintf_s(file, "%s\n", wr.c_str());
		}
	}
	fprintf_s(file, "};");
	fclose(file);
	return std::string(fileName);
}

std::string Graph::generateDotCodeForResult(const char * fileNamePrefix, int * stepCount, const std::vector<ExecutionState *> * states, ExecutionState * result)
{
	char tmp[256];
	char fileName[256];
	FILE * file;
	sprintf_s(fileName, 256, "%s%d.dot", fileNamePrefix, (*stepCount)++);
	if (fopen_s(&file, fileName, "w"))
		return std::string("");

	fprintf_s(file, "digraph {\nrankdir = LR;\n");
	// ������ ����.
	std::vector<std::string> nodestrings;
	for (size_t i = 0; i < states->size(); i++)
	{
		ExecutionState * state = (*states)[i];
		sprintf_s(tmp, 256, "\"%d\\n len=%d\"", state->node->number, state->totalWeight);
		nodestrings.push_back(std::string(tmp));

		// ���������� ���� � ����.
		std::string wr(tmp);
		wr.append(";");
		fprintf_s(file, "%s\n", wr.c_str());
	}
	// ������ ��������.
	for (size_t i = 0; i < states->size(); i++)
	{
		ExecutionState * state = (*states)[i];
		std::vector<Edge> edges = state->node->edges;
		for (size_t j = 0; j < edges.size(); j++)
		{
			std::string wr = nodestrings[edges[j].from->number] + " -> " + nodestrings[edges[j].to->number];

			// ����������� �� ���� ��������������� ����?
			bool belongsToResult = false;
			for (size_t k = 0; !belongsToResult && k < result->path.size(); k++)
				if (result->path[k] == edges[j])
					belongsToResult = true;

			if (belongsToResult)
				sprintf_s(tmp, 256, "[label=\"%d\", color=magenta];", edges[j].weight);	// �������� ����, ������������� ����, ������� ������.
			else
				sprintf_s(tmp, 256, "[label=\"%d\"];", edges[j].weight);

			wr.append(tmp);
			fprintf_s(file, "%s\n", wr.c_str());
		}
	}
	fprintf_s(file, "};");
	fclose(file);
	return std::string(fileName);
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