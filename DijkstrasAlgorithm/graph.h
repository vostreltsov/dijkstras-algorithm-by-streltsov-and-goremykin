#pragma once
#include <stdio.h>
#include <map>
#include <vector>
#include <string>

struct Node;

/**
 * ������� ������ �� �����.
 */
struct FileListItem
{
	std::string from;	// ������ ����.
	std::string to;		// ����� ����.
	__int64 weight;		// ��� ����.

	FileListItem();
	FileListItem(const std::string _from, const std::string _to, const __int64 _weight);
	bool operator==(const FileListItem & other) const;
};

/**
 * ���� ����� ����� ������.
 */
struct Edge
{
	Node * from;	// ������ ����.
	Node * to;		// ����� ����.
	__int64 weight;	// ��� ����.

	Edge();
	Edge(Node * _from, Node * _to, const __int64 _weight);
	bool operator==(const Edge & other) const;
};

/**
 * ���� � �����.
 */
struct Node
{
	std::string name;			// ��� ����.
	std::vector<Edge *> edges;	// ����, ��������� �� ����� ����.

	Node();
	Node(const std::string _name);
	~Node();
};

/**
 * ��������������� ��������� ��� ���������� ���������.
 * ������ ����� ��������� �������� � ������������ ������� ���� ����� ��� ���������� ���������.
 */
struct ExecutionState
{
	Node * node;				// ��������������� ���� � �����.
	__int64 totalWeight;		// ����� ���� �� ����.
	bool passed;				// ������� �� ����.
	std::vector<Edge *> path;	// ���� �� ��������� ������� �� this->node.

	ExecutionState();
	ExecutionState(const Node * _node);
};

/**
 * ����.
 */
class Graph
{
private:
	std::map<std::string, Node *> nodes;	// ���� �����.
	std::vector<int> errors;				// ��������� "������" � �����.
	Node * startNode;						// ��������� ������� ��������.
	Node * endNode;							// �������� ������� ��������.

#ifdef _DEBUG
	friend class TestSuite;
#endif

	/**
	 * ������ ���� �� ��������� �� ����� ������.
	 * @param edges - ������ �������� FileListItem.
	 */
	void build(std::vector<FileListItem> edges);

	/**
	 * ��������� ��������� ������ �� �������������� ������������: ��������������� ��� ��� � ���������� ������.
	 * ��������������� ������� ����������� ���� errors.
	 * @param edges - ������ �������� Edge. � ���� �������� ������ ���������� Node * ������������ ������� �����.
	 * @param start - ��������� ������� ��������.
	 * @param end - �������� ������� ��������.
	 */
	void validate(std::vector<FileListItem> edges, const std::string start, const std::string end);

public:
	// ��������� ���� ������������� ��������.
	static const int ERROR_NOT_EXISTS = 0;
	// � ��������� ����� ���� ���� � ������������� �����.
	static const int ERROR_NEGATIVE_WEIGHT = 1;
	// � ��������� ����� ���� �����.
	static const int ERROR_LOOP_EXISTS = 2;
	// ������� �������������� ������� ����.
	static const int ERROR_WRONG_PATH_BORDERS = 3;
	// �� ������� ������� ����.
	static const int ERROR_COULD_NOT_OPEN_FILE = 4;

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
	 * ����������.
	 */
	~Graph();

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
	 * @param fileNamePrefix - ������� ��� ���� ������������ ������, ������� ������ ���� �� ���.
	 * @param dotFilesGenerated - ��������� �� ������, � ������� ��������� ����� ��������������� ������. ������ ���������� �����������, ���������� �������� ������ ����� ������ �������.
	 * @return - ������ ExecutionState, ���������� ������ ���������������� ��������� �� ������� start � ������� end � ��������� ����� ����.
	 */
	ExecutionState run(const char * fileNamePrefix, std::vector<std::string> * dotFilesGenerated);

	/**
	 * ��������� ����� � ��������� ����� (�� �����-�� ���� ���������) �� ����� dot.
	 * ���������� ������� ������������ ���������, ������������ - �������� ������.
	 * ������� ������� ���������� ������� ������, ���������� �������� ����� ������, ������������ - ������.
	 * @param fileNamePrefix - ������� ����� dot-����� �� ������, ���������� ������ ����.
	 * @param stepCount - ��������� ����������-������� ��������������� ������.
	 * @param states - ��������� �� ������� ��������� ����������.
	 * @param currentEdge - ������� ���� �����.
	 * @return - ��� ���������������� �����.
	 */
	std::string Graph::generateDotCodeForStep(const char * fileNamePrefix, int * stepCount, const std::map<std::string, ExecutionState *> * states, const Edge * currentEdge);

	/**
	 * ��������� ����� � ��������� ����� (��� ���������� ����������) �� ����� dot.
	 * ��������, ������������� ��������������� ����, ���������� ������� ������, ��������� - ������.
	 * @param fileNamePrefix - ������� ����� dot-����� �� ������, ���������� ������ ����.
	 * @param stepCount - ��������� ����������-������� ��������������� ������.
	 * @param states - ��������� �� ������� ��������� ����������.
	 * @param result - ��������� �� ��������� ������ ���������.
	 * @return - ��� ���������������� �����.
	 */
	std::string generateDotCodeForResult(const char * fileNamePrefix, int * stepCount, const std::map<std::string, ExecutionState *> * states, ExecutionState * result);
};