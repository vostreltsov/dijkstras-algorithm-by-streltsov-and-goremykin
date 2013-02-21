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
	readFromFile(fileName);	// Валидация происходит внутри этой функции.
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

	// Строим связи между узлами.
	for (size_t i = 0; i < edges.size(); i++)
	{
		// Добавляем узлы, если их еще нет в графе.
		if (nodes.find(edges[i].from) == nodes.end())
			nodes.insert(std::pair<std::string, Node *>(edges[i].from, new Node(edges[i].from)));
		if (nodes.find(edges[i].to) == nodes.end())
			nodes.insert(std::pair<std::string, Node *>(edges[i].to, new Node(edges[i].to)));

		// Добавляем дугу между узлами.
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
		// Есть ли дуги с отрицательным весом?
		if (edges[i].weight < 0)
			negativeWeight = true;
		// Есть ли петли?
		if (edges[i].from == edges[i].to)
			loopExists = true;
		if (edges[i].from == start || edges[i].to == start)
			startExists = true;
		if (edges[i].from == end || edges[i].to == end)
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
	__int64 m = 0;				// Число дуг в графе.
	char buf1[256] = "";		// Буфер для чтения строк.
	char buf2[256] = "";		// Буфер для чтения строк.
	std::string pathStart = "";	// Начальная вершина маршрута.
	std::string pathEnd = "";	// Конечная вершина маршрута.

	FILE * file;
	if (fopen_s(&file, fileName, "r"))
	{
		errors.clear();
		errors.push_back(Graph::ERROR_COULD_NOT_OPEN_FILE);
		return false;
	}

	// Читаем количество дуг, имена начального и конечного узлов маршрута.
	fscanf_s(file, "%I64d", &m);
	fscanf_s(file, "%s", buf1);
	fscanf_s(file, "%s", buf2);
	pathStart = buf1;
	pathEnd = buf2;

	// Оставшаяся часть файла - информация о дугах.
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
	int stepCount = 0;								// Счетчик сгенерированных картинок.
	std::map<std::string, ExecutionState *> states;	// Каждому узлу в графе ставится в соответствие объект ExecutionState.
	ExecutionState * startState = NULL;				// Начальное состояние при выполнении.

	// Создаем объект ExecutionState для каждого узла графа и запоминаем начальное состояние.
	for (std::map<std::string, Node *>::const_iterator iter = nodes.cbegin(); iter != nodes.cend(); iter++)
	{
		ExecutionState * newState = new ExecutionState(iter->second);
		if (newState->node == startNode)
			startState = newState;
		states.insert(std::pair<std::string, ExecutionState *>(newState->node->name, newState));
	}
	startState->totalWeight = 0;

	// Генерируем файл в начале выполнения алгоритма.
	dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, NULL));

	// Выполняем алгоритм.
	ExecutionState * currentState = startState;	// Вершина с минимальной меткой.
	while (currentState != NULL)
	{
		// Просматриваем всех соседей текущей вершины.
		for (size_t i = 0; i < currentState->node->edges.size(); i++)
		{
			Edge * edge = currentState->node->edges[i];
			ExecutionState * destState = states[edge->to->name];	// Cостояние, соответствующее конечной вершине ребра.

			// Перезаписываем путь до конечной вершины текущей дуги.
			if (currentState->totalWeight != -1 && (destState->totalWeight == -1 || destState->totalWeight > currentState->totalWeight + edge->weight))
			{
				destState->path = currentState->path;
				destState->path.push_back(edge);
				destState->totalWeight = currentState->totalWeight + edge->weight;
			}

			// Генерируем файл в середине выполнения алгоритма.
			dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, edge));

		}
		// Помечаем вершину как пройденную и выбираем новую вершину с минимальной меткой.
		currentState->passed = true;
		currentState = NULL;
		for (std::map<std::string, ExecutionState *>::const_iterator iter = states.cbegin(); iter != states.cend(); iter++)
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
		dotFilesGenerated->push_back(generateDotCodeForStep(fileNamePrefix, &stepCount, &states, NULL));
	}

	// Формируем результат.
	ExecutionState result;
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states.cbegin(); iter != states.cend(); iter++)
		if (!iter->second->path.empty() && iter->second->path.front()->from == startNode && iter->second->path.back()->to == endNode)
			result = *iter->second;

	// Генерируем файл, в котором отображается оптимальный путь.
	if (result.path.size() > 0)
		dotFilesGenerated->push_back(generateDotCodeForResult(fileNamePrefix, &stepCount, &states, &result));

	//  Очищаем выделенную память.
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
	// Задаем узлы.
	std::map<std::string, std::string> nodestrings;
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		sprintf_s(tmp, 256, "\"%s\\n len=%I64d\"", state->node->name.c_str(), state->totalWeight);
		nodestrings.insert(std::pair<std::string, std::string>(state->node->name.c_str(), std::string(tmp)));

		// Записываем узел в файл, выделяя пройденное состояние пунктиром.
		std::string wr(tmp);
		if (state->passed)
			wr.append("[style=dotted]");
		wr.append(";");
		fprintf_s(file, "%s\n", wr.c_str());
	}
	// Задаем переходы.
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		std::vector<Edge *> edges = state->node->edges;
		for (size_t j = 0; j < edges.size(); j++)
		{
			std::string wr = nodestrings.find(edges[j]->from->name)->second + " -> " + nodestrings.find(edges[j]->to->name)->second;

			if (edges[j] == currentEdge)
				sprintf_s(tmp, 256, "[label=\"%I64d\", color=red];", edges[j]->weight);		// Выделяем текущую дугу красным цветом.
			else if (states->find(edges[j]->from->name)->second->passed)
				sprintf_s(tmp, 256, "[label=\"%I64d\", color=blue];", edges[j]->weight);	// Выделяем пройденную дугу синим цветом.
			else
				sprintf_s(tmp, 256, "[label=\"%I64d\"];", edges[j]->weight);				// Остальные дуги никак не выделяем.

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
	// Задаем узлы.
	std::map<std::string, std::string> nodestrings;
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		sprintf_s(tmp, 256, "\"%s\\n len=%I64d\"", state->node->name.c_str(), state->totalWeight);
		nodestrings.insert(std::pair<std::string, std::string>(state->node->name.c_str(), std::string(tmp)));

		// Записываем узел в файл.
		std::string wr(tmp);
		wr.append(";");
		fprintf_s(file, "%s\n", wr.c_str());
	}
	// Задаем переходы.
	for (std::map<std::string, ExecutionState *>::const_iterator iter = states->cbegin(); iter != states->cend(); iter++)
	{
		ExecutionState * state = iter->second;
		std::vector<Edge *> edges = state->node->edges;
		for (size_t j = 0; j < edges.size(); j++)
		{
			std::string wr = nodestrings.find(edges[j]->from->name)->second + " -> " + nodestrings.find(edges[j]->to->name)->second;

			// Принадлежит ли дуга результирующему пути?
			bool belongsToResult = false;
			for (size_t k = 0; !belongsToResult && k < result->path.size(); k++)
				if (result->path[k] == edges[j])
					belongsToResult = true;

			if (belongsToResult)
				sprintf_s(tmp, 256, "[label=\"%I64d\", color=magenta];", edges[j]->weight);	// Выделяем дугу, принадлежащую пути, зеленым цветом.
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