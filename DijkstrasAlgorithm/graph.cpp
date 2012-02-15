#include "graph.h"

Transition::Transition(Node * _from, Node * _to, const int _weight)
{
	from = _from;
	to = _to;
	weight = _weight;
}

Transition::Transition()
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
}

Graph::Graph(const char * fileName)
{
	readFromFile(fileName);	// Валидация происходит внутри этой функции.
}

void Graph::validate()
{
	errors.clear();
	// TODO найти петли, дуги с отрицательным весом и т.д.
}

bool Graph::readFromFile(const char * fileName)
{
	// TODO придумать формат файла, написать тут что-то.
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

std::vector<Transition> Graph::run(const int start, const int end)
{
	return std::vector<Transition>();
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