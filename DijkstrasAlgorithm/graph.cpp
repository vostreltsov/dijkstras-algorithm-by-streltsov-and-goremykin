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

Graph::~Graph()
{
	for (std::map<int, Node *>::const_iterator iter = nodes.cbegin(); iter != nodes.cend(); iter++)
		delete iter->second;
}

void Graph::build(std::vector<Edge> edges)
{
	for (std::map<int, Node *>::const_iterator iter = nodes.cbegin(); iter != nodes.cend(); iter++)
		delete iter->second;
	nodes.clear();

	// Строим связи между узлами.
	for (size_t i = 0; i < edges.size(); i++)
	{
		// Добавляем узлы, если их еще нет в графе.
		int from = (int)edges[i].from;
		int to = (int)edges[i].to;
		if (nodes.find(from) == nodes.end())
			nodes.insert(std::pair<int, Node *>(from, new Node(from)));
		if (nodes.find(to) == nodes.end())
			nodes.insert(std::pair<int, Node *>(to, new Node(to)));

		// Добавляем дугу между узлами.
		Node * fromPtr = nodes.find(from)->second;
		Node * toPtr = nodes.find(to)->second;
		fromPtr->edges.push_back(Edge(fromPtr, toPtr, edges[i].weight));
	}
}

void Graph::validate(std::vector<Edge> edges, const int start, const int end)
{
	bool negativeWeight = false;
	bool loopExists = false;
	bool startExists = false;
	bool endExists = false;

	for (size_t i = 0; i < edges.size(); i++)
	{
		// Есть ли дуги с отрицательным весом?
		if (edges[i].weight < 0)
			negativeWeight = true;
		// Есть ли петли?
		if (edges[i].from == edges[i].to)
			loopExists = true;
		if ((int)edges[i].from == start || (int)edges[i].to == start)
			startExists = true;
		if ((int)edges[i].from == end || (int)edges[i].to == end)
			endExists = true;
	}

	// Заполняем вектор ошибок.
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
	int m = 0;			// Число дуг в графе.
	int pathStart = 0;	// Начальная вершина маршрута.
	int pathEnd = 0;	// Конечная вершина маршрута.

	FILE * file;
	if (fopen_s(&file, fileName, "r"))
	{
		errors.clear();
		errors.push_back(Graph::ERROR_COULD_NOT_OPEN_FILE);
		return false;
	}

	// Читаем количество дуг, узлов, номера начального и конечного узлов маршрута.
	fscanf_s(file, "%d\n %d %d\n", &m, &pathStart, &pathEnd);

	// Оставшаяся часть файла - информация о дугах.
	std::vector<Edge> edges;
	for (int i = 0; i < m; i++)
	{
		int edgeStart = 0;
		int edgeEnd = 0;
		int edgeWeight = 0;
		fscanf_s(file, "%d %d %d", &edgeStart, &edgeEnd, &edgeWeight);
		edges.push_back(Edge((Node *)edgeStart, (Node *)edgeEnd, edgeWeight));	// При валидации вместо указателей - индексы.
	}
	fclose(file);

	// Проверяем считанные данные и строим граф, если все нормально.
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
		return "Ошибок нет";
	case ERROR_NEGATIVE_WEIGHT:
		return "Найдена дуга с отрицательным весом";
	case ERROR_LOOP_EXISTS:
		return "Найдена петля";
	case ERROR_WRONG_PATH_BORDERS:
		return "Начальная или конечная вершина не существует в графе";
	case ERROR_COULD_NOT_OPEN_FILE:
		return "Не удалось открыть файл";
	default:
		return "Неизвестная ошибка";
	}
}

ExecutionState Graph::run(const char * fileNamePrefix, std::vector<std::string> * dotFilesGenerated)
{
	int stepCount = 0;						// Счетчик сгенерированных картинок.
	std::map<int, ExecutionState *> states;	// Каждому узлу в графе ставится в соответствие объект ExecutionState.
	ExecutionState * startState = NULL;		// Начальное состояние при выполнении.

	// Создаем объект ExecutionState для каждого узла графа и запоминаем начальное состояние.
	for (std::map<int, Node *>::const_iterator iter = nodes.cbegin(); iter != nodes.cend(); iter++)
	{
		ExecutionState * newState = new ExecutionState(iter->second);
		if (newState->node == startNode)
			startState = newState;
		states.insert(std::pair<int, ExecutionState *>(newState->node->number, newState));
	}
	startState->totalWeight = 0;

	// Генерируем файл в начале выполнения алгоритма.
	dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, Edge()));

	// Выполняем алгоритм.
	ExecutionState * currentState = startState;	// Вершина с минимальной меткой.
	while (currentState != NULL)
	{
		// Просматриваем всех соседей текущей вершины.
		for (size_t i = 0; i < currentState->node->edges.size(); i++)
		{
			Edge edge = currentState->node->edges[i];
			ExecutionState * destState = states[edge.to->number];	// Cостояние, соответствующее конечной вершине ребра.

			// Перезаписываем путь до конечной вершины текущей дуги.
			if (currentState->totalWeight != -1 && (destState->totalWeight == -1 || destState->totalWeight > currentState->totalWeight + edge.weight))
			{
				destState->path = currentState->path;
				destState->path.push_back(edge);
				destState->totalWeight = currentState->totalWeight + edge.weight;
			}

			// Генерируем файл в середине выполнения алгоритма.
			dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, edge));

		}
		// Помечаем вершину как пройденную и выбираем новую вершину с минимальной меткой.
		currentState->passed = true;
		currentState = NULL;
		for (std::map<int, ExecutionState *>::const_iterator iter = states.cbegin(); iter != states.cend(); iter++)
		{
			ExecutionState * tmp = iter->second;
			if (!tmp->passed)
			{
				// Выбираем текущую вершину, если она пока что равна NULL или найдена вершина с меньшей меткой.
				if (currentState == NULL ||
					(tmp->totalWeight != -1 && currentState->totalWeight > tmp->totalWeight) ||
					(currentState->totalWeight == -1 && tmp->totalWeight != -1))
					currentState = tmp;
			}
		}
		// Генерируем файл после прохождения очередной вершины.
		dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, Edge()));
	}

	// Формируем результат.
	ExecutionState result;
	for (std::map<int, ExecutionState *>::const_iterator iter = states.cbegin(); iter != states.cend(); iter++)
		if (!iter->second->path.empty() && iter->second->path.front().from == startNode && iter->second->path.back().to == endNode)
			result = *iter->second;

	// Генерируем файл, в котором отображается оптимальный путь.
	if (result.path.size() > 0)
		dotFilesGenerated->push_back(generateDotCodeForResult(fileNamePrefix, &stepCount, &states, &result));

	//  Очищаем выделенную память.
	for (size_t i = 0; i < states.size(); i++)
		delete states[i];

	return result;
}

std::string Graph::generateDotCodeForStep(const char * fileNamePrefix, int * stepCount, const std::map<int, ExecutionState *> * states, const Edge currentEdge)
{
	char tmp[256];
	char fileName[256];
	FILE * file;
	sprintf_s(fileName, 256, "%s%d.dot", fileNamePrefix, (*stepCount)++);
	if (fopen_s(&file, fileName, "w"))
		return std::string("");

	fprintf_s(file, "digraph {\nrankdir = LR;\n");
	// Задаем узлы.
	std::map<int, std::string> nodestrings;
	for (std::map<int, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		sprintf_s(tmp, 256, "\"%d\\n len=%d\"", state->node->number, state->totalWeight);
		nodestrings.insert(std::pair<int, std::string>(state->node->number, std::string(tmp)));

		// Записываем узел в файл, выделяя пройденное состояние пунктиром.
		std::string wr(tmp);
		if (state->passed)
			wr.append("[style=dotted]");
		wr.append(";");
		fprintf_s(file, "%s\n", wr.c_str());
	}
	// Задаем переходы.
	for (std::map<int, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		std::vector<Edge> edges = state->node->edges;
		for (size_t j = 0; j < edges.size(); j++)
		{
			std::string wr = nodestrings.find(edges[j].from->number)->second + " -> " + nodestrings.find(edges[j].to->number)->second;

			if (edges[j] == currentEdge)
				sprintf_s(tmp, 256, "[label=\"%d\", color=red];", edges[j].weight);		// Выделяем текущую дугу красным цветом.
			else if (states->find(edges[j].from->number)->second->passed)
				sprintf_s(tmp, 256, "[label=\"%d\", color=blue];", edges[j].weight);	// Выделяем пройденную дугу синим цветом.
			else
				sprintf_s(tmp, 256, "[label=\"%d\"];", edges[j].weight);				// Остальные дуги никак не выделяем.

			wr.append(tmp);
			fprintf_s(file, "%s\n", wr.c_str());
		}
	}
	fprintf_s(file, "};");
	fclose(file);
	return std::string(fileName);
}

std::string Graph::generateDotCodeForResult(const char * fileNamePrefix, int * stepCount, const std::map<int, ExecutionState *> * states, ExecutionState * result)
{
	char tmp[256];
	char fileName[256];
	FILE * file;
	sprintf_s(fileName, 256, "%s%d.dot", fileNamePrefix, (*stepCount)++);
	if (fopen_s(&file, fileName, "w"))
		return std::string("");

	fprintf_s(file, "digraph {\nrankdir = LR;\n");
	// Задаем узлы.
	std::map<int, std::string> nodestrings;
	for (std::map<int, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		sprintf_s(tmp, 256, "\"%d\\n len=%d\"", state->node->number, state->totalWeight);
		nodestrings.insert(std::pair<int, std::string>(state->node->number, std::string(tmp)));

		// Записываем узел в файл.
		std::string wr(tmp);
		wr.append(";");
		fprintf_s(file, "%s\n", wr.c_str());
	}
	// Задаем переходы.
	for (std::map<int, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		std::vector<Edge> edges = state->node->edges;
		for (size_t j = 0; j < edges.size(); j++)
		{
			std::string wr = nodestrings.find(edges[j].from->number)->second + " -> " + nodestrings.find(edges[j].to->number)->second;

			// Принадлежит ли дуга результирующему пути?
			bool belongsToResult = false;
			for (size_t k = 0; !belongsToResult && k < result->path.size(); k++)
				if (result->path[k] == edges[j])
					belongsToResult = true;

			if (belongsToResult)
				sprintf_s(tmp, 256, "[label=\"%d\", color=magenta];", edges[j].weight);	// Выделяем дугу, принадлежащую пути, зеленым цветом.
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