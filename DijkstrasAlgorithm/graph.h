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
	bool operator==(const Edge & other) const;
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
 * ��������������� ��������� ��� ���������� ���������.
 * ������ ����� ��������� �������� � ������������ ������� ���� ����� ��� ���������� ���������.
 */
struct ExecutionState
{
	Node * node;			// ��������� �������� � ������������ ���� �����.
	int totalWeight;		// ����� ���� �� ����.
	bool passed;			// ������� �� ����.
	std::vector<Edge> path;	// ���� �� ��������� ������� �� this->node.
	
	ExecutionState();
	ExecutionState(const Node * _node);
};

/**
 * ����.
 */
class Graph
{
private:
	std::vector<Node> nodes;	// ���� �����.
	std::vector<int> errors;	// ��������� "������" � �����.
	Node * startNode;			// ��������� ������� ��������.
	Node * endNode;				// �������� ������� ��������.

#ifdef _DEBUG
	//class TestSuite;
	friend class TestSuite;
#endif

	/**
	 * ������ ���� �� ��������� �� ����� ������.
	 * @param vertexCount - ���������� ������.
	 * @param edges - ������ �������� Edge. � ���� �������� ������ ���������� Node * ������������ ������� �����.
	 */
	void build(const int vertexCount, std::vector<Edge> edges);

	/**
	 * ��������� ��������� ������ �� �������������� ������������: ��������������� ��� ��� � ���������� ������.
	 * ��������������� ������� ����������� ���� errors.
	 * @param vertexCount - ���������� ������.
	 * @param edges - ������ �������� Edge. � ���� �������� ������ ���������� Node * ������������ ������� �����.
	 * @param start - ��������� ������� ��������.
	 * @param end - �������� ������� ��������.
	 */
	void validate(const int vertexCount, std::vector<Edge> edges, const int start, const int end);

public:
	// ��������� ���� ������������� ��������.
	static const int ERROR_NOT_EXISTS = 0;
	// � ��������� ����� ���� ���� � ������������� �����.
	static const int ERROR_NEGATIVE_WEIGHT = 1;
	// � ��������� ����� ���� �����.
	static const int ERROR_LOOP_EXISTS = 2;
	// ���� �������� ����� ����, ������� ��� ���������� �����.
	static const int ERROR_UNKNOWN_NODE = 3;
	// ������� �������������� ������� ����.
	static const int ERROR_PATH_BORDERS_NOT_EXIST = 4;

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
	 * @return - true, ���� ���� ������ ������, ����� false. ���������� ���������� ����� �� �������� ���������� ���������� ������ � ���.
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
	 * ��������� "������" � ���� ���������.
	 * @param errorCode - ��� ������.
	 * @return - ��������������� ������.
	 */
	static char * getErrorString(const int errorCode);

	/**
	 * ���������� ��������� ��������.
	 * @return - ������ ExecutionState, ���������� ������ ���������������� ��������� �� ������� start � ������� end � ��������� ����� ����.
	 */
	ExecutionState run();

	/**
	 * ��������� ����� � ��������� ����� �� ����� dot.
	 * ���������� ������� ������������ ���������, ������������ - �������� ������.
	 * ������� ������� ���������� ������� ������, ���������� �������� ����� ������, ������������ - ������.
	 * @param fileName - ��� dot-����� �� ������.
	 * @param states - ��������� �� ������ �������� ��������� ����������.
	 * @param currentEdge - ��������� �� ������� ���� �����.
	 * @return - true, ���� ���� ������� ������������, ����� false.
	 */
	bool generateDotCode(const char * fileName, const std::vector<ExecutionState *> * states, const Edge currentEdge);
};