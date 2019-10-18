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

	IntArray(int size=1, const char * name = "");

	~IntArray() {
	}

	bool ensure(int size);

	int getLength();

	int get(int pos) {
		return data[pos];
	}
	
	void set(int pos, int elem) {
		ensure(pos+1);
		data[pos] = elem;
		if (pos >= curSize) 
			curSize = pos + 1;
	}

	void set_unsafe(int pos, int elem) {
		data[pos] = elem;
		curSize = pos+1;
	}

	//attention je ne deplace pas le curseur de taille
	int & operator() ( int pos ) {
		ensure(pos+1);
		if (pos >= curSize) curSize = pos + 1;
		return data[pos];
	}

	//attention je ne deplace pas le curseur de taille
	int & operator[] (int pos) {
		ensure(pos+1);
		if (pos >= curSize) curSize = pos + 1;
		return data[pos];
	}

	void push_back(int elem);
	void push_front(int elem);
	void insert(int pos, int elem);

	// 0 1 3 
	// si je cherche 2
	// 0 1 <2> 3 
	// renvoie la position apres l'element 1
	int searchPosition( int element ) {
		for (int i = 0; i < getLength(); i++)
			if (element <= data[i])
				return i;
		return getLength();
	}

	void removeAll() {
		curSize = 0;
	}

	/**
	0 1 3 supprime 1 
	reste 0 3 
	*/
	bool remove(int valeur) {
		int idx = -1;
		for (int i = 0; i < getLength(); i++) {
			if (data[i] == valeur) {
				idx = i;
				break;
			}
		}
		if (idx == -1) return false;
		for (int i = idx; i < getLength()-1; i++)
			data[i] = data[i+1];
		data[curSize-1] = 0;
		curSize--;
	}

	bool remove2(int valeur) {
		int idx = -1;
		for (int i = 0; i < getLength(); i++) {
			if (data[i] == valeur) {
				for (int j = idx; j < getLength() - 1; j++)
					data[j] = data[j + 1];
				data[curSize - 1] = 0;
				curSize--;
				return true;
			}
		}
		return false;
	}

	void fillWithRandom(int nbElem) {
		ensure(nbElem);
		for (int i = 0; i < nbElem; ++i) {
			int val = std::rand();
			set(i, val );
		}
	}

	void sort() {
		IntArray nuData(1,"nudata");
		for (int i = 0; i < getLength(); i++) {
			int val = get(i);
			int pos = nuData.searchPosition(val);
			nuData.insert(nuData.searchPosition(val),val);
		}

		for (int i = 0; i < getLength(); i++) {
			set(i,nuData.get(i));
		}
		//inserer tout les elements au bon endroit un par un
		//

		//creer un tableau temporaire
		//inserer dans l'ordre dans ce nouveau tableau
		//recuperer ce nouveau tableau et détruire le courant
	}

	void swap(int pos0, int pos1) 
	{
		int save = data[pos0];
		data[pos0] = data[pos1];
		data[pos1] = save;		
	}

	void insertSort() {
		for (int i = 1; i < curSize; i++) {
			int j = i;
			while (j>0 && data[j]<data[j-1])
			{
				swap(j, j-1);
				j--;
			}
		}
	}

	/// 4 
	// 3 parcours * 2 reparcours et permutation  

	// n 
	// 2 n  

	// 5

	// 0 2 3 4 5
	// 1 1 2 3 4

	// 5 parcours sur i
		// 1 parcours entre i et 0 

	// 5 *4		 // 20 + 12 + 6 ~ 38 ~ 5*5
	// 4 *3    +
	// 3 * 2  +
	// 1 ...  +

	//6 * 5 + 38 = 68
	

	// tableau de taille[n]
	// n-1  parcours * n -2 parcours
	// n^² 

	// renvoie la position ou on devrait s'inserer
	int binarySearch(int key) {
		//demander la recherche entre 0 et la taille
		return _binarySearch(key, 0, curSize);
	}


	int _binarySearch(int key, int start, int end) {
		printf("%d s:%d e:%d\n", key, start, end);
		if (end <= start) return start;
		if (end == start + 1) {
			if (key > data[start])	return end;
			if (key < data[end])	return start;
			return start;
		}

		int pivot = (end + start) / 2;
		if(key == data[pivot])return pivot;

		if (key < data[pivot])
			return _binarySearch(key, start, pivot);
		else 
			return _binarySearch(key, pivot, end);
	}
};
