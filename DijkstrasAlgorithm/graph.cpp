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

	FILE * file = fopen(fileName, "r");
	if (file == NULL)
		return false;
	
	// Читаем количество узлов, номера начального и конечного узлов маршрута.
	fscanf(file, "%d", &n);
	fscanf(file, "%d", &pathStart);
	fscanf(file, "%d", &pathEnd);

	// Оставшаяся часть файла - информация о дугах.
	std::vector<Edge> edges;
	while (!feof(file))
	{
		int edgeStart = 0;
		int edgeEnd = 0;
		int edgeWeight = 0;
		fscanf(file, "%d %d %d", &edgeStart, &edgeEnd, &edgeWeight);
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

std::vector<Edge> Graph::run(const int start, const int end)
{
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

	// Выполняем алгоритм.

	// Очищаем выделенную память.
	for (int i = 0; i < states.size(); i++)
		delete states[i];
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
	passed = false;
}

ExecutionState::ExecutionState(const Node * _node)
{
	node = const_cast<Node *>(_node);
	passed = false;
}