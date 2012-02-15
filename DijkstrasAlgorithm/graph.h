#pragma once
#include <stdio.h>
#include <vector>

struct Node;

/**
 * ���� ����� ����� ������.
 */
struct Transition
{
	Node * from;	// ������ ����.
	Node * to;		// ����� ����.
	int weight;		// ��� ����.
};

/**
 * ���� � �����.
 */
struct Node
{
	int number;								// ����� ����.
	std::vector<Transition> transitions;	// ����, ��������� �� ����� ����.
};

/**
 * ����.
 */
class Graph
{
private:
	std::vector<Node> nodes;	// ���� �����.
	std::vector<int> errors;	// ��������� "������" � �����.
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
	 * ��������� ���� �� �����.
	 * @param fileName - ��� �����, � �������� ���������.
	 * @return - true, ���� ���� ������ ������, ����� false.
	 */
	bool readFromFile(const char * fileName);

	/**
	 * ��������� ���� �� �������������� ������������: ��������������� ��� ��� � ���������� ������.
	 * ��������������� ������� ����������� ���� errors.
	 */
	void validate();

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
	std::vector<Transition> run(const int start, const int end);
};

/**
 * ��������������� ��������� ��� ���������� ���������.
 * ������ ����� ��������� �������� � ������������ ������� ���� ����� ��� ���������� ���������.
 */
struct ExecutionState
{
	Node * node;					// ��������� �������� � ������������ ���� �����.
	int totalWeight;				// ����� ���� �� ����.
	std::vector<Transition> path;	// ���� �� ��������� ������� �� this->node.
};