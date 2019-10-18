
#include "pch.h"
#include "IntArray.hpp"

int IntArray::TEST = 66;

IntArray::IntArray(int size, const char * name) {
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

bool IntArray::ensure(int size) {
	if (size < maxSize) return false;

	int* newdata = new int[size];

	for (int i = 0; i < size; ++i) newdata[i] = 0;
	for (int i = 0; i < curSize; i++)
		newdata[i] = data[i];

	delete(data);
	data = newdata;

	maxSize = size;
	//printf("on passe");
	return true;
}

int IntArray::getLength() {
	return curSize;
}


void IntArray::push_back(int elem) {
	ensure(curSize + 1);
	data[curSize] = elem;
	curSize++;
}

void IntArray::push_front(int elem) {
	ensure(curSize + 1);
	for (int i = curSize; i > 0; i--) 
		data[i] = data[i - 1];
	data[0] = elem;
	curSize++;
}

void IntArray::insert(int pos, int elem) {
	/**
	if( ( pos>=curSize ) ){
		ensure(pos+1);
	else
		ensure( curSize +1 );
	*/
	ensure( ( pos>=curSize ) ? (pos + 1) : (curSize+1) );

	for (int i = curSize; i > pos; i--) 
		data[i] = data[i - 1];

	data[pos] = elem;

	if( pos >= curSize )
		curSize = pos + 1;
	else 
		curSize++;
}
