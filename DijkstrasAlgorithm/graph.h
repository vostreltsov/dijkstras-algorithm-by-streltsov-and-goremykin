#pragma once
#include <stdio.h>
#include <vector>

struct Node;

/**
 * ���� ����� ����� ������.
 */
struct Edge
{
	Node * from;	// ������ ����.
	Node * to;		// ����� ����.
	int weight;		// ��� ����.

	Edge();
	Edge(Node * _from, Node * _to, const int _weight);
};

/**
 * ���� � �����.
 */
struct Node
{
	int number;					// ����� ����.
	std::vector<Edge> edges;	// ����, ��������� �� ����� ����.
	
	Node();
	Node(const int _number);
};

/**
 * ����.
 */
class Graph
{
private:
	std::vector<Node> nodes;	// ���� �����.
	std::vector<int> errors;	// ��������� "������" � �����.

	/**
	 * ��������� ���� �� �������������� ������������: ��������������� ��� ��� � ���������� ������.
	 * ��������������� ������� ����������� ���� errors.
	 */
	void validate();

public:
	// ��������� ���� ������������� ��������.
	static const int ERROR_NOT_EXISTS = 0;
	// � ��������� ����� ���� ���� � ������������� �����.
	static const int ERROR_NEGATIVE_WEIGHT = 1;
	// � ��������� ����� ���� �����.
	static const int ERROR_LOOP_EXISTS = 2;
	// ���� �������� ����� ����, ������� ��� ���������� �����.
	static const int ERROR_UNKNOWN_NODE = 3;

	/**
	 * ����������� �� ���������.
	 */
	Graph();

	/**
	 * �����������, � ������� ����� �������� ���� �� ��������� �����.
	 * @param fileName - ��� �����, � �������� ���������.
	 */
	Graph(const char * fileName);

	/**
	 * ��������� ���� �� �����.
	 * @param fileName - ��� �����, � �������� ���������.
	 * @return - true, ���� ���� ������ ������, ����� false.
	 */
	bool readFromFile(const char * fileName);

	/**
	 * ������������� �� ���� ��������?
	 * @return - true, ���� �������������, ����� false.
	 */
	bool error_exists();

	/**
	 * ��������� "������", ��������� � �����.
	 * @return - ������ � ������ ������.
	 */
	std::vector<int> getErrors();

	/**
	 * ���������� ��������� ��������.
	 * @param start - ��������� �������.
	 * @param end - �������� �������.
	 * @return - ������ ���������������� ��������� �� ������� start � ������� end.
	 */
	std::vector<Edge> run(const int start, const int end);
};

/**
 * ��������������� ��������� ��� ���������� ���������.
 * ������ ����� ��������� �������� � ������������ ������� ���� ����� ��� ���������� ���������.
 */
struct ExecutionState
{
	Node * node;			// ��������� �������� � ������������ ���� �����.
	int totalWeight;		// ����� ���� �� ����.
	std::vector<Edge> path;	// ���� �� ��������� ������� �� this->node.
	
	ExecutionState();
	ExecutionState(const Node * _node);
};