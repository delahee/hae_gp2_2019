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

		//version c++ plus safe
		data = new int[size];
		for (int i = 0; i < size; ++i) data[i] = 0;

		//cursize = 0;
		maxSize = size;
	}

	~IntArray() {
		printf("detruite! %s\n", name.c_str());
	}
};
