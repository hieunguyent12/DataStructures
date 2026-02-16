#include "BTree.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <cassert>

BTree::BTree(int maxElementsPerNode) :
	maxElementsPerNode(maxElementsPerNode),
	minElementsPerNode(maxElementsPerNode / 2),
	order(maxElementsPerNode + 1),
	height(0)
{
}

BTree::~BTree()
{
}

void BTree::insert(int key, int value)
{
	Element* element = new Element(key, value);

	if (root == nullptr) {
		root = new BTreeNode();
		root->isLeaf = true;
		root->insertElement(element);
		height = 1;
		return;
	}

	_insert(root, element);
}

BTreeNode* BTree::_insert(BTreeNode* node, Element* element)
{
	if (node->isLeaf) {
		node->insertElement(element);

		if (node->elements.size() > maxElementsPerNode) {
			int median = node->elements.size() / 2;

			BTreeNode* a = node->split(0, median);
			BTreeNode* b = node->split(median + 1, node->elements.size());
			a->isLeaf = true;
			b->isLeaf = true;

			BTreeNode* tempNode = new BTreeNode();

			tempNode->insertElement(node->elements[median]);
			tempNode->children.push_back(a);
			tempNode->children.push_back(b);

			if (node == root) {
				delete root;
				root = tempNode;
				root->isLeaf = false;
				height++;
				return nullptr;
			}

			return tempNode;
		}

		return nullptr;
	}

	// Traverse the BTree to find the leaf node to insert
	int i = 0;
	while (*element > *(node->elements[i])) {
		i++;
		if (i >= node->elements.size()) break;
	}
	BTreeNode* separationNode = _insert(node->children[i], element);

	if (node == root) {
		if (height == 1) {
			delete root;
			root = separationNode;
			root->isLeaf = false;
			height++;
			return nullptr;
		}
	}

	if (separationNode != nullptr) {
		// "merge" the separation node to the current node
		int insertIdx = node->insertElement(separationNode->elements[0]);

		node->children.erase(node->children.begin() + i);

		for (int i = 0; i < separationNode->children.size(); i++) {
			node->children.insert(node->children.begin() + i + insertIdx, separationNode->children[i]);
		}

		delete separationNode;
	}

	if (node->elements.size() > maxElementsPerNode) {
		int median = node->elements.size() / 2;

		BTreeNode* a = node->split(0, median);
		BTreeNode* b = node->split(median + 1, node->elements.size());

		BTreeNode* tempNode = new BTreeNode();

		tempNode->insertElement(node->elements[median]);
		tempNode->children.push_back(a);
		tempNode->children.push_back(b);

		if (node == root) {
			delete root;
			root = tempNode;
			root->isLeaf = false;
			height++;
			return nullptr;
		}

		return tempNode;
	}

	return nullptr;
}

void BTree::remove(int key)
{
	_remove(root, key);
}

// https://www.cs.rhodes.edu/~kirlinp/courses/db/f16/handouts/btrees-deletion.pdf
void BTree::_remove(BTreeNode* node, int key)
{
	if (node->isLeaf) {
		for (int i = 0; i < node->elements.size(); i++) {
			if (node->elements[i]->key == key) {
				delete node->elements[i];
				node->elements.erase(node->elements.begin() + i);
				return;
			}
		}

		return;
	}

	bool found = false;
	int childIdx = 0;
	for (int i = 0; i <= node->elements.size(); i++) {
		if (i == node->elements.size()) {
			childIdx = i;
			break;
		}

		if (key < node->elements[i]->key) {
			childIdx = i;
			break;
		}

		if (key == node->elements[i]->key) {
			node->elements.erase(node->elements.begin() + i);
			found = true;
			childIdx = i;
			break;
		}
	}

	if (found) {
		BTreeNode* leftSubtree = node->children[childIdx];

		if (leftSubtree == nullptr) {
			return;
		}

		if (leftSubtree->elements.empty()) {
			return;
		}

		node->elements.insert(node->elements.begin() + childIdx, leftSubtree->elements[leftSubtree->elements.size() - 1]);
		leftSubtree->elements.pop_back();

		if (leftSubtree->elements.size() < minElementsPerNode) {
			int separatorIdx = childIdx;

			_rebalance(leftSubtree, node, separatorIdx);
		}
	}
	else {
		BTreeNode* childNode = node->children[childIdx];
		_remove(childNode, key);

		if (childNode->elements.size() < minElementsPerNode) {
			int separatorIdx = childIdx;

			_rebalance(childNode, node, separatorIdx);
		}
	}
}

void BTree::_rebalance(BTreeNode* node, BTreeNode* parentNode, int separatorIdx)
{
	BTreeNode* leftSiblingNode = nullptr;
	BTreeNode* rightSiblingNode = nullptr;

	if (separatorIdx + 1 < parentNode->children.size()) {
		if (parentNode->children[separatorIdx + 1] != nullptr) {
			rightSiblingNode = parentNode->children[separatorIdx + 1];
		}
	}

	if (separatorIdx - 1 > 0) {
		if (parentNode->children[separatorIdx - 1] != nullptr) {
			leftSiblingNode = parentNode->children[separatorIdx - 1];
		}
	}

	if (rightSiblingNode && rightSiblingNode->elements.size() > minElementsPerNode) {
		node->elements.push_back(parentNode->elements[separatorIdx]);
		node->elements[separatorIdx] = rightSiblingNode->elements[0];
		rightSiblingNode->elements.erase(rightSiblingNode->elements.begin());
	}
	else if (leftSiblingNode && leftSiblingNode->elements.size() > minElementsPerNode) {
		node->elements.insert(node->elements.begin(), parentNode->elements[separatorIdx]);
		node->elements[separatorIdx] = leftSiblingNode->elements[leftSiblingNode->elements.size() - 1];
		leftSiblingNode->elements.pop_back();
	}
	else {
		BTreeNode* leftNode;
		BTreeNode* rightNode;

		if (rightSiblingNode) {
			leftNode = node;
			rightNode = rightSiblingNode;
		}
		else {
			leftNode = leftSiblingNode;
			rightNode = node;
		}

		assert(leftNode != nullptr && rightNode != nullptr && "Left node and right node can not be null");

		leftNode->elements.push_back(parentNode->elements[separatorIdx]);

		for (Element* element : rightNode->elements) {
			leftNode->elements.push_back(element);
		}

		parentNode->elements.erase(parentNode->elements.begin() + separatorIdx);
		parentNode->children.erase(parentNode->children.begin() + separatorIdx);

		if (parentNode == root && parentNode->elements.empty()) {
			delete root;
			root = leftNode;

			if (root->children.empty()) {
				root->isLeaf = true;
			}

			height--;
		}
	}
}

void BTree::find(int key)
{
}

void BTree::print()
{
	_print(std::vector<BTreeNode*>{root}, 0);
}

// TODO: implement print using a queue, not sure which approach is more intuitive
void BTree::_print(std::vector<BTreeNode*> nodes, int height)
{
	if (nodes.empty()) return;

	std::cout << "Height " << std::to_string(height) << ": ";

	std::vector<BTreeNode*> children;

	for (BTreeNode* node : nodes) {
		std::cout << "[ ";
		for (Element* element : node->elements) {
			std::cout << element->key << " ";
		}
		std::cout << "] ";

		for (BTreeNode* child : node->children) {
			children.push_back(child);
		}
	}

	std::cout << std::endl;

	_print(children, height + 1);
}

int BTreeNode::findElementInsertIndex(Element* element)
{
	if (elements.size() == 0) return 0;

	int i = 0;
	while (*element > *(elements[i])) {
		i++;
		if (i >= elements.size()) break;
	};

	return i;
}

BTreeNode::BTreeNode() : isLeaf(false)
{
}

BTreeNode::~BTreeNode()
{
}

int BTreeNode::insertElement(Element* element)
{
	int index = findElementInsertIndex(element);
	elements.insert(elements.begin() + index, element);

	return index;
}

void BTreeNode::insertChild(BTreeNode* childNode, int index)
{
	children.insert(children.begin() + index, childNode);
}

BTreeNode* BTreeNode::split(int begin, int end)
{
	BTreeNode* newNode = new BTreeNode();

	// copy elements
	for (int i = begin; i < end; i++) {
		newNode->insertElement(elements[i]);
	}

	// copy children
	for (int i = begin; i <= end; i++) {
		if (i >= children.size()) break;
		newNode->children.push_back(children[i]);
	}

	return newNode;
}
