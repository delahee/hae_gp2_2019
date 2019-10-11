#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>

class IntArray {

public:
	std::string name;
	int * data; //tableau dynamique qui va contenir nos données
	int curSize = 0;
	int maxSize = 0;

	//membre TEST statique ( qui n'existe que à un seul endroit )
	static int TEST;

	IntArray( int size , const char * name = ""){
		//on passe name en std::string
		//alloue automatiquement un char* et fait la copie
		this->name = name;
		printf("construite! %s\n", this->name.c_str());

		//version C old school
		//data = (int * )malloc(size * sizeof(int));
		//memset(data,0, size * sizeof(int));

		//version C new school
		//data = (int * )calloc(size , sizeof(int));
		//calloc mets des zero
		if (size == 0) size++;
		//version c++ plus safe
		data = new int[size];
		for (int i = 0; i < size; ++i) data[i] = 0;

		//cursize = 0;
		maxSize = size;
	}

	~IntArray() {
		printf("detruite! %s\n", name.c_str());
	}

	bool ensure(int size);

	int getLength();

	int get(int pos) {
		return data[pos];
	}
	
	void set(int pos, int elem) {
		ensure(pos+1);
		data[pos] = elem;
		curSize = pos+1;
	}

	void set_unsafe(int pos, int elem) {
		data[pos] = elem;
		curSize = pos+1;
	}

	int operator() ( int pos ) {
		ensure(pos+1);
		return data[pos];
	}

	int & operator[] (int pos) {
		ensure(pos+1);
		return data[pos];
	}

	void push_back(int elem);
	void push_front(int elem);
	void insert(int pos, int elem);


};
