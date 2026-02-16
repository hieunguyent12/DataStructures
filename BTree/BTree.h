#pragma once

#include <vector>

using namespace std;

// An element represents a key-value pair in the tree
struct Element {
	int key;
	int value;

	Element(int key, int value) : key(key), value(value) {}

	bool operator<(const Element& other) const {
		return key < other.key;
	}

	bool operator>(const Element& other) const {
		return key > other.key;
	}
};

class BTreeNode {
private:
	int findElementInsertIndex(Element* element);
public:
	BTreeNode();
	~BTreeNode();

	bool isLeaf;
	// Each node has at least ceil(order/2) - 1 elements or (order-1)/2
	vector<Element*> elements;

	// Every non-leaf node (except root) has at least ceil(order/2) children
	vector<BTreeNode*> children;

	int insertElement(Element* element);
	void insertChild(BTreeNode* childNode, int index);
	BTreeNode* split(int begin, int end);
};

class BTree {
private:
	BTreeNode* root = nullptr;

	BTreeNode* _insert(BTreeNode* node, Element* element);
	void _remove(BTreeNode* node, int key);
	void _rebalance(BTreeNode* node, BTreeNode* parentNode, int separatorIdx);
	void _print(std::vector<BTreeNode*> nodes, int height);
public:
	// The number of children each node can have (which is one more than the maximum number of keys per node)
	int order;
	int maxElementsPerNode;
	int minElementsPerNode;
	int height;

	BTree(int maxElementsPerNode = 3);
	~BTree();

	// https://en.wikipedia.org/wiki/B-tree#Insertion
	void insert(int key, int value);
	// https://en.wikipedia.org/wiki/B-tree#Deletion
	void remove(int key);
	void find(int key);
	void print();
};