#pragma once

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
				right->insertLeft(newElem);
			else
				right->insertRight(newElem);
		}
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
			this->root == new Node(newElem);
		else
			root->insert(newElem);
	}
};

typedef Node<float> FloatNode;
typedef Tree<float> FloatTree;

