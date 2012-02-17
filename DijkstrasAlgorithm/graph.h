#pragma once
#include <stdio.h>
#include <vector>

struct Node;

/**
 * Дуга между двумя узлами.
 */
struct Edge
{
	Node * from;	// Начало дуги.
	Node * to;		// Конец дуги.
	int weight;		// Вес дуги.

	Edge();
	Edge(Node * _from, Node * _to, const int _weight);
	bool operator==(const Edge & other) const;
};

/**
 * Узел в графе.
 */
struct Node
{
	int number;					// Номер узла.
	std::vector<Edge> edges;	// Дуги, выходящие из этого узла.
	
	Node();
	Node(const int _number);
};

/**
 * Вспомогательная структура для выполнения алгоритма.
 * Объект такой структуры ставится в соответствие каждому узлу графа при выполнении алгоритма.
 */
struct ExecutionState
{
	Node * node;			// Состоянию ставится в соответствие узел графа.
	int totalWeight;		// Длина пути до узла.
	bool passed;			// Пройден ли узел.
	std::vector<Edge> path;	// Путь от начальной вершины до this->node.
	
	ExecutionState();
	ExecutionState(const Node * _node);
};

/**
 * Граф.
 */
class Graph
{
private:
	std::vector<Node> nodes;	// Узлы графа.
	std::vector<int> errors;	// Найденные "ошибки" в графе.
	Node * startNode;			// Начальная вершина маршрута.
	Node * endNode;				// Конечная вершина маршрута.

#ifdef _DEBUG
	//class TestSuite;
	friend class TestSuite;
#endif

	/**
	 * Строит граф из считанных из файла данных.
	 * @param vertexCount - количество вершин.
	 * @param edges - вектор объектов Edge. В этих объектах вместо указателей Node * используются индексы узлов.
	 */
	void build(const int vertexCount, std::vector<Edge> edges);

	/**
	 * Проверяет считанные данные на удовлетворение ограничениям: неотрицательный вес дуг и отсутствие петель.
	 * Соответствующим образом заполняется поле errors.
	 * @param vertexCount - количество вершин.
	 * @param edges - вектор объектов Edge. В этих объектах вместо указателей Node * используются индексы узлов.
	 * @param start - начальная вершина маршрута.
	 * @param end - конечная вершина маршрута.
	 */
	void validate(const int vertexCount, std::vector<Edge> edges, const int start, const int end);

public:
	// Считанный граф удовлетворяет условиям.
	static const int ERROR_NOT_EXISTS = 0;
	// В считанном графе есть дуги с отрицательным весом.
	static const int ERROR_NEGATIVE_WEIGHT = 1;
	// В считанном графе есть петли.
	static const int ERROR_LOOP_EXISTS = 2;
	// Дуга содержит номер узла, больший чем количество узлов.
	static const int ERROR_UNKNOWN_NODE = 3;
	// Указаны несуществующие границы пути.
	static const int ERROR_PATH_BORDERS_NOT_EXIST = 4;

	/**
	 * Конструктор по умолчанию.
	 */
	Graph();

	/**
	 * Конструктор, в котором сразу строится граф по заданному файлу.
	 * @param fileName - имя файла, с которого считывать.
	 */
	Graph(const char * fileName);

	/**
	 * Считывает граф из файла.
	 * @param fileName - имя файла, с которого считывать.
	 * @return - true, если граф считан удачно, иначе false. Корректное считывание файла не означает отсутствие логических ошибок в нем.
	 */
	bool readFromFile(const char * fileName);

	/**
	 * Удовлетворяет ли граф условиям?
	 * @return - true, если удовлетворяет, иначе false.
	 */
	bool error_exists();

	/**
	 * Получение "ошибок", найденных в графе.
	 * @return - вектор с кодами ошибок.
	 */
	std::vector<int> getErrors();

	/**
	 * Получение "ошибки" в виде сообщения.
	 * @param errorCode - код ошибки.
	 * @return - соответствующая строка.
	 */
	static char * getErrorString(const int errorCode);

	/**
	 * Выполнение алгоритма Дейкстры.
	 * @return - объект ExecutionState, содержащий вектор последовательных переходов из вершины start в вершину end и суммарную длину пути.
	 */
	ExecutionState run();

	/**
	 * Генерация файла с описанием графа на языке dot.
	 * Пройденные вершины обозначаются пунктиром, непройденные - сплошной линией.
	 * Текущий переход выделяется красным цветом, пройденные переходы синим цветом, непройденные - черным.
	 * @param fileName - имя dot-файла на выходе.
	 * @param states - указатель на вектор текущего состояния выполнения.
	 * @param currentEdge - указатель на текущую дугу графа.
	 * @return - true, если файл успешно сгенерирован, иначе false.
	 */
	bool generateDotCode(const char * fileName, const std::vector<ExecutionState *> * states, const Edge currentEdge);
};