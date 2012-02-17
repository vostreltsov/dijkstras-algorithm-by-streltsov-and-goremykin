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
	number = -1;	// Нумерация узлов в графе идет с нуля.
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
	readFromFile(fileName);	// Валидация происходит внутри этой функции.
}

void Graph::build(const int vertexCount, std::vector<Edge> edges)
{
	nodes.clear();
	// Добавляем vertexCount узлов в граф.
	for (int i = 0; i < vertexCount; i++)
		nodes.push_back(Node(i));
	// Строим связи между узлами.
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
		// Есть ли дуги с отрицательным весом?
		if (edges[i].weight < 0)
			negativeWeight = true;
		// Есть ли петли?
		if (edges[i].from == edges[i].to)
			loopExists = true;
		// Не указана ли несуществующая вершина?
		if ((int)edges[i].from < 0 || (int)edges[i].from >= vertexCount || (int)edges[i].to < 0 || (int)edges[i].to >= vertexCount)
			unknownNode = true;
	}

	// Заполняем вектор ошибок.
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
	int n = 0;			// Число вершин в графе.
	int pathStart = 0;	// Начальная вершина маршрута.
	int pathEnd = 0;	// Конечная вершина маршрута.

	FILE * file;
	if (fopen_s(&file, fileName, "r"))
		return false;
	
	// Читаем количество узлов, номера начального и конечного узлов маршрута.
	fscanf_s(file, "%d", &n);
	fscanf_s(file, "%d", &pathStart);
	fscanf_s(file, "%d", &pathEnd);

	// Оставшаяся часть файла - информация о дугах.
	std::vector<Edge> edges;
	while (!feof(file))
	{
		int edgeStart = 0;
		int edgeEnd = 0;
		int edgeWeight = 0;
		fscanf_s(file, "%d %d %d", &edgeStart, &edgeEnd, &edgeWeight);
		edges.push_back(Edge((Node *)edgeStart, (Node *)edgeEnd, edgeWeight));	// При валидации вместо указателей - индексы.
	}
	fclose(file);

	// Проверяем считанные данные и строим граф, если все нормально.
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
	default:
		return "Unknown error";
	}
}

ExecutionState Graph::run()
{
	int stepCount = 0;
	char dotFileName[256] = "";

	// Создаем объект ExecutionState для каждого узла графа и запоминаем начальное состояние.
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
	sprintf_s(dotFileName, 256, "C:\\step%d.dot", stepCount++);
	generateDotCode(dotFileName, &states, Edge());

	// Выполняем алгоритм.
	ExecutionState * currentState = startState;	// Вершина с минимальной меткой.
	while (currentState != NULL)
	{
		// Просматриваем всех соседей текущей вершины.
		for (int i = 0; i < currentState->node->edges.size(); i++)
		{
			Edge edge = currentState->node->edges[i];
			ExecutionState * destState = states[edge.to->number];	// Cостояние, соответствующее конечной вершине ребра.

			// Перезаписываем путь до конечной вершины текущей дуги.
			if (destState->totalWeight == -1 || destState->totalWeight > currentState->totalWeight + edge.weight)
			{
				destState->path = currentState->path;
				destState->path.push_back(edge);
				destState->totalWeight = currentState->totalWeight + edge.weight;
			}
			
			sprintf_s(dotFileName, 256, "C:\\step%d.dot", stepCount++);
			generateDotCode(dotFileName, &states, edge);

		}		
		// Помечаем вершину как пройденную и выбираем новую вершину с минимальной меткой.
		currentState->passed = true;
		currentState = NULL;
		for (int i = 0; i < states.size(); i++)
		{
			ExecutionState * tmp = states[i];
			if (!tmp->passed)
			{
				// Выбираем текущую вершину, если она пока что равна NULL или найдена вершина с меньшей меткой.
				if (currentState == NULL || (tmp->totalWeight != -1 && currentState->totalWeight > tmp->totalWeight))
					currentState = tmp;
			}
		}
	}
	sprintf_s(dotFileName, 256, "C:\\step%d.dot", stepCount++);
	generateDotCode(dotFileName, &states, Edge());

	// Формируем результат и очищаем выделенную память.
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
	// Задаем узлы.
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
			strcat_s(tmp, 256, "[style=dotted]");	// Выделяем пройденное состояние пунктиром.
		strcat_s(tmp, 256, ";");
		
		fprintf_s(file, "%s\n", tmp);
	}
	// Задаем переходы.
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
				sprintf_s(extra, 256, "[label=\"%d\", color=red];", edges[j].weight);	// Выделяем текущую дугу красным цветом.
			else if ((*states)[edges[j].from->number]->passed)
				sprintf_s(extra, 256, "[label=\"%d\", color=blue];", edges[j].weight);	// Выделяем пройденную дугу синим цветом.
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