#pragma once

#include <cstdio>
#include <functional>

template<typename T>
class Node {
public:
	T		elem;
	Node *	left	= nullptr;
	Node *	right	= nullptr;

	Node(const T&elem) {
		this->elem = elem;
	}

	~Node() {
		if(left) delete left;
		if(right) delete right;
		left = nullptr;
		right = nullptr;
	}

	void insert(const T & newElem) {
		if (newElem < this->elem)
			insertLeft(newElem);
		else 
			insertRight(newElem);
	}

	void insertLeft(const T & newElem) {
		if (left == nullptr)
			left = new Node(newElem);
		else {
			if (left->elem < newElem)
				left->insertRight(newElem);
			else
				left->insertLeft(newElem);
		}
	}

	void insertRight(const T & newElem) {
		if (right == nullptr)
			right = new Node(newElem);
		else {
			if (right->elem < newElem)
				right->insertRight(newElem);
			else
				right->insertLeft(newElem);
		}
	}
	bool Contains(const T & newElem)
	{
		if (elem == newElem) return true;
		if (elem > newElem)
		{
			if(left)  return left->Contains(newElem);
		}
		if (elem < newElem)
		{
			if(right) return right->Contains(newElem);
		}
		return false;
	}

	void InsertNode(Node* newBranch) {
		if (newBranch == nullptr) return;
		auto r = newBranch->right;
		auto l = newBranch->left;
		newBranch->left = nullptr;
		newBranch->right = nullptr;
		insert(newBranch->elem);
		delete newBranch;
		InsertNode(l);
		InsertNode(r);
	}

	Node* remove(const T & oldElem) {
		Node* l = left;
		Node* r = right;

		if (left) left = left->remove(oldElem);
		if (right) right = right->remove(oldElem);

		if (elem == oldElem)
		{
			left = nullptr;
			right = nullptr;
			delete this;
			if (l == nullptr) return r;
			l->InsertNode(r);
			return l;
		}
		else
		{
			return this;
		}
	}

	int getLength() {
		int count = 1;
		if (left != nullptr)
			count += left->getLength();
		if (right != nullptr)
			count += right->getLength();
		return count;
	}

	void dfsPrint() {
		if (left)left->dfsPrint();
		printf("%d ", elem);
		if (right)right->dfsPrint();
	}

	void dfsPrintReversed() {
		if (right)right->dfsPrintReversed();
		printf("%d ", elem);
		if (left)left->dfsPrintReversed();
	}

	void dfs(std::function<void(Node*)> fun) {

	}

	void bfs(std::function<void(Node*)> fun) {

	}

	void bfsPrint() {
		printf("%d ", elem);
		if (left)left->bfsPrint();
		if (right)right->bfsPrint();
	}
};

template<typename T>
class Tree
{
public:
	Node<T> * root = nullptr;

	Tree() {

	}

	~Tree() {
		delete root;
		root = nullptr;
	}

	void insert(const T & newElem) {
		if (root == nullptr)
			this->root == new Node<T>(newElem);
		else
			root->insert(newElem);
	}

	void remove(const T & newElem) {
		if (root) root->remove(newElem);
	}

	int getLength() {
		return root == nullptr ? 0 : root->getLength();
	}

	void dfsPrint() {
		if (root) root->dfsPrint();
		printf("\n");
	}

	void dfsPrintReversed() {
		if (root) root->dfsPrintReversed();
		printf("\n");
	}

	void bfsPrint() {
		if (root) root->bfsPrint();
		printf("\n");
	}
};

typedef Node<float> FloatNode;
typedef Tree<float> FloatTree;
typedef Tree<int> IntTree;
typedef Node<int> IntNode;

