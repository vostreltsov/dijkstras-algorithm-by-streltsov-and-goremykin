#include "graph.h"

FileListItem::FileListItem()
{
	from = "";
	to = "";
	weight = 0;
}

FileListItem::FileListItem(const std::string _from, const std::string _to, const __int64 _weight)
{
	from = _from;
	to = _to;
	weight = _weight;
}

bool FileListItem::operator==(const FileListItem & other) const
{
	return (from == other.from && to == other.to && weight == other.weight);
}

/*----------------------------------------------------------------------------------------------------*/

Edge::Edge()
{
	from = NULL;
	to = NULL;
	weight = 0;
}

Edge::Edge(Node * _from, Node * _to, const __int64 _weight)
{
	from = _from;
	to = _to;
	weight = _weight;
}

bool Edge::operator==(const Edge & other) const
{
	return (from == other.from && to == other.to && weight == other.weight);
}

/*----------------------------------------------------------------------------------------------------*/

Node::Node()
{
	name = "";
}

Node::Node(const std::string _name)
{
	name = _name;
}

Node::~Node()
{
	for (size_t i = 0; i < edges.size(); i++)
		delete edges[i];
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

Graph::~Graph()
{
	for (std::map<std::string, Node *>::const_iterator iter = nodes.cbegin(); iter != nodes.cend(); iter++)
		delete iter->second;
}

void Graph::build(std::vector<FileListItem> edges)
{
	for (std::map<std::string, Node *>::const_iterator iter = nodes.cbegin(); iter != nodes.cend(); iter++)
		delete iter->second;
	nodes.clear();

	// ������ ����� ����� ������.
	for (size_t i = 0; i < edges.size(); i++)
	{
		// ��������� ����, ���� �� ��� ��� � �����.
		if (nodes.find(edges[i].from) == nodes.end())
			nodes.insert(std::pair<std::string, Node *>(edges[i].from, new Node(edges[i].from)));
		if (nodes.find(edges[i].to) == nodes.end())
			nodes.insert(std::pair<std::string, Node *>(edges[i].to, new Node(edges[i].to)));

		// ��������� ���� ����� ������.
		Node * fromPtr = nodes.find(edges[i].from)->second;
		Node * toPtr = nodes.find(edges[i].to)->second;
		fromPtr->edges.push_back(new Edge(fromPtr, toPtr, edges[i].weight));
	}
}

void Graph::validate(std::vector<FileListItem> edges, const std::string start, const std::string end)
{
	bool negativeWeight = false;
	bool loopExists = false;
	bool startExists = false;
	bool endExists = false;

	for (size_t i = 0; i < edges.size(); i++)
	{
		// ���� �� ���� � ������������� �����?
		if (edges[i].weight < 0)
			negativeWeight = true;
		// ���� �� �����?
		if (edges[i].from == edges[i].to)
			loopExists = true;
		if (edges[i].from == start || edges[i].to == start)
			startExists = true;
		if (edges[i].from == end || edges[i].to == end)
			endExists = true;
	}

	// ��������� ������ ������.
	errors.clear();
	if (negativeWeight)
		errors.push_back(Graph::ERROR_NEGATIVE_WEIGHT);
	if (loopExists)
		errors.push_back(Graph::ERROR_LOOP_EXISTS);
	if (!startExists || !endExists)
		errors.push_back(Graph::ERROR_WRONG_PATH_BORDERS);
}

bool Graph::readFromFile(const char * fileName)
{
	__int64 m = 0;				// ����� ��� � �����.
	char buf1[256] = "";		// ����� ��� ������ �����.
	char buf2[256] = "";		// ����� ��� ������ �����.
	std::string pathStart = "";	// ��������� ������� ��������.
	std::string pathEnd = "";	// �������� ������� ��������.

	FILE * file;
	if (fopen_s(&file, fileName, "r"))
	{
		errors.clear();
		errors.push_back(Graph::ERROR_COULD_NOT_OPEN_FILE);
		return false;
	}

	// ������ ���������� ���, ����� ���������� � ��������� ����� ��������.
	fscanf_s(file, "%I64d", &m);
	fscanf_s(file, "%s", buf1);
	fscanf_s(file, "%s", buf2);
	pathStart = buf1;
	pathEnd = buf2;

	// ���������� ����� ����� - ���������� � �����.
	std::vector<FileListItem> edges;
	for (__int64 i = 0; i < m; i++)
	{
		__int64 edgeWeight = 0;
		fscanf_s(file, "%s", buf1);
		fscanf_s(file, "%s", buf2);
		fscanf_s(file, "%I64d", &edgeWeight);
		edges.push_back(FileListItem(buf1, buf2, edgeWeight));
	}
	fclose(file);

	// ��������� ��������� ������ � ������ ����, ���� ��� ���������.
	validate(edges, pathStart, pathEnd);
	if (errors.empty())
	{
		build(edges);
		startNode = nodes.find(pathStart)->second;
		endNode = nodes.find(pathEnd)->second;
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
	case ERROR_WRONG_PATH_BORDERS:
		return "��������� ��� �������� ������� �� ���������� � �����";
	case ERROR_COULD_NOT_OPEN_FILE:
		return "�� ������� ������� ����";
	default:
		return "����������� ������";
	}
}

ExecutionState Graph::run(const char * fileNamePrefix, std::vector<std::string> * dotFilesGenerated)
{
	int stepCount = 0;								// ������� ��������������� ��������.
	std::map<std::string, ExecutionState *> states;	// ������� ���� � ����� �������� � ������������ ������ ExecutionState.
	ExecutionState * startState = NULL;				// ��������� ��������� ��� ����������.

	// ������� ������ ExecutionState ��� ������� ���� ����� � ���������� ��������� ���������.
	for (std::map<std::string, Node *>::const_iterator iter = nodes.cbegin(); iter != nodes.cend(); iter++)
	{
		ExecutionState * newState = new ExecutionState(iter->second);
		if (newState->node == startNode)
			startState = newState;
		states.insert(std::pair<std::string, ExecutionState *>(newState->node->name, newState));
	}
	startState->totalWeight = 0;

	// ���������� ���� � ������ ���������� ���������.
	dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, NULL));

	// ��������� ��������.
	ExecutionState * currentState = startState;	// ������� � ����������� ������.
	while (currentState != NULL)
	{
		// ������������� ���� ������� ������� �������.
		for (size_t i = 0; i < currentState->node->edges.size(); i++)
		{
			Edge * edge = currentState->node->edges[i];
			ExecutionState * destState = states[edge->to->name];	// C��������, ��������������� �������� ������� �����.

			// �������������� ���� �� �������� ������� ������� ����.
			if (currentState->totalWeight != -1 && (destState->totalWeight == -1 || destState->totalWeight > currentState->totalWeight + edge->weight))
			{
				destState->path = currentState->path;
				destState->path.push_back(edge);
				destState->totalWeight = currentState->totalWeight + edge->weight;
			}

			// ���������� ���� � �������� ���������� ���������.
			dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, edge));

		}
		// �������� ������� ��� ���������� � �������� ����� ������� � ����������� ������.
		currentState->passed = true;
		currentState = NULL;
		for (std::map<std::string, ExecutionState *>::const_iterator iter = states.cbegin(); iter != states.cend(); iter++)
		{
			ExecutionState * tmp = iter->second;
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
		dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, NULL));
	}

	// ��������� ���������.
	ExecutionState result;
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states.cbegin(); iter != states.cend(); iter++)
		if (!iter->second->path.empty() && iter->second->path.front()->from == startNode && iter->second->path.back()->to == endNode)
			result = *iter->second;

	// ���������� ����, � ������� ������������ ����������� ����.
	if (result.path.size() > 0)
		dotFilesGenerated->push_back(generateDotCodeForResult(fileNamePrefix, &stepCount, &states, &result));

	//  ������� ���������� ������.
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states.cbegin(); iter != states.cend(); iter++)
		delete iter->second;

	return result;
}

std::string Graph::generateDotCodeForStep(const char * fileNamePrefix, int * stepCount, const std::map<std::string, ExecutionState *> * states, const Edge * currentEdge)
{
	char tmp[256];
	char fileName[256];
	FILE * file;
	sprintf_s(fileName, 256, "%s%d.dot", fileNamePrefix, (*stepCount)++);
	if (fopen_s(&file, fileName, "w"))
		return std::string("");

	fprintf_s(file, "digraph {\nrankdir = LR;\n");
	// ������ ����.
	std::map<std::string, std::string> nodestrings;
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		sprintf_s(tmp, 256, "\"%s\\n len=%I64d\"", state->node->name.c_str(), state->totalWeight);
		nodestrings.insert(std::pair<std::string, std::string>(state->node->name.c_str(), std::string(tmp)));

		// ���������� ���� � ����, ������� ���������� ��������� ���������.
		std::string wr(tmp);
		if (state->passed)
			wr.append("[style=dotted]");
		wr.append(";");
		fprintf_s(file, "%s\n", wr.c_str());
	}
	// ������ ��������.
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		std::vector<Edge *> edges = state->node->edges;
		for (size_t j = 0; j < edges.size(); j++)
		{
			std::string wr = nodestrings.find(edges[j]->from->name)->second + " -> " + nodestrings.find(edges[j]->to->name)->second;

			if (edges[j] == currentEdge)
				sprintf_s(tmp, 256, "[label=\"%I64d\", color=red];", edges[j]->weight);		// �������� ������� ���� ������� ������.
			else if (states->find(edges[j]->from->name)->second->passed)
				sprintf_s(tmp, 256, "[label=\"%I64d\", color=blue];", edges[j]->weight);	// �������� ���������� ���� ����� ������.
			else
				sprintf_s(tmp, 256, "[label=\"%I64d\"];", edges[j]->weight);				// ��������� ���� ����� �� ��������.

			wr.append(tmp);
			fprintf_s(file, "%s\n", wr.c_str());
		}
	}
	fprintf_s(file, "};");
	fclose(file);
	return std::string(fileName);
}

std::string Graph::generateDotCodeForResult(const char * fileNamePrefix, int * stepCount, const std::map<std::string, ExecutionState *> * states, ExecutionState * result)
{
	char tmp[256];
	char fileName[256];
	FILE * file;
	sprintf_s(fileName, 256, "%s%d.dot", fileNamePrefix, (*stepCount)++);
	if (fopen_s(&file, fileName, "w"))
		return std::string("");

	fprintf_s(file, "digraph {\nrankdir = LR;\n");
	// ������ ����.
	std::map<std::string, std::string> nodestrings;
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		sprintf_s(tmp, 256, "\"%s\\n len=%I64d\"", state->node->name.c_str(), state->totalWeight);
		nodestrings.insert(std::pair<std::string, std::string>(state->node->name.c_str(), std::string(tmp)));

		// ���������� ���� � ����.
		std::string wr(tmp);
		wr.append(";");
		fprintf_s(file, "%s\n", wr.c_str());
	}
	// ������ ��������.
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		std::vector<Edge *> edges = state->node->edges;
		for (size_t j = 0; j < edges.size(); j++)
		{
			std::string wr = nodestrings.find(edges[j]->from->name)->second + " -> " + nodestrings.find(edges[j]->to->name)->second;

			// ����������� �� ���� ��������������� ����?
			bool belongsToResult = false;
			for (size_t k = 0; !belongsToResult && k < result->path.size(); k++)
				if (result->path[k] == edges[j])
					belongsToResult = true;

			if (belongsToResult)
				sprintf_s(tmp, 256, "[label=\"%I64d\", color=magenta];", edges[j]->weight);	// �������� ����, ������������� ����, ������� ������.
			else
				sprintf_s(tmp, 256, "[label=\"%I64d\"];", edges[j]->weight);

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