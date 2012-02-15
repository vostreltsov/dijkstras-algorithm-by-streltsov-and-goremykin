#pragma once
#include <stdio.h>
#include <vector>

struct Node;

/**
 * Дуга между двумя узлами.
 */
struct Transition
{
	Node * from;	// Начало дуги.
	Node * to;		// Конец дуги.
	int weight;		// Вес дуги.
};

/**
 * Узел в графе.
 */
struct Node
{
	int number;								// Номер узла.
	std::vector<Transition> transitions;	// Дуги, выходящие из этого узла.
};

/**
 * Граф.
 */
class Graph
{
private:
	std::vector<Node> nodes;	// Узлы графа.
	std::vector<int> errors;	// Найденные "ошибки" в графе.
public:
	// Считанный граф удовлетворяет условиям.
	static const int ERROR_NOT_EXISTS = 0;
	// В считанном графе есть дуги с отрицательным весом.
	static const int ERROR_NEGATIVE_WEIGHT = 1;
	// В считанном графе есть петли.
	static const int ERROR_LOOP_EXISTS = 2;
	// Дуга содержит номер узла, больший чем количество узлов.
	static const int ERROR_UNKNOWN_NODE = 3;

	/**
	 * Считывает граф из файла.
	 * @param fileName - имя файла, с которого считывать.
	 * @return - true, если граф считан удачно, иначе false.
	 */
	bool readFromFile(const char * fileName);

	/**
	 * Проверяет граф на удовлетворение ограничениям: неотрицательный вес дуг и отсутствие петель.
	 * Соответствующим образом заполняется поле errors.
	 */
	void validate();

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
	 * Выполнение алгоритма Дейкстры.
	 * @param start - начальная вершина.
	 * @param end - конечная вершина.
	 * @return - вектор последовательных переходов из вершины start в вершину end.
	 */
	std::vector<Transition> run(const int start, const int end);
};

/**
 * Вспомогательная структура для выполнения алгоритма.
 * Объект такой структуры ставится в соответствие каждому узлу графа при выполнении алгоритма.
 */
struct ExecutionState
{
	Node * node;					// Состоянию ставится в соответствие узел графа.
	int totalWeight;				// Длина пути до узла.
	std::vector<Transition> path;	// Путь от начальной вершины до this->node.
};