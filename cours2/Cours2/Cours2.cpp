// Cours2.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "pch.h"
#include <iostream>
#include "IntArray.hpp"
#include <time.h>



void StrCat(char * dst, const char * src) {
	if (*dst == 0) {
		if (*src == 0)
			return;
		else {
			*dst = *src;
			*(dst + 1) = 0;
			StrCat(dst + 1, src+1);
		}
	}
	else
		StrCat(dst + 1, src);
}

int StrCmp(const char * s0, const char * s1) {
	if (*s0 == 0 && *s1 == 0) return 0;
	else if (*s0 < *s1) return 1;
	else if (*s0 > *s1) return -1;
	return StrCmp(s0 + 1, s1 + 1);
}

int _StrChr(const char * s0, char s1, const char * _start) {
	if (*s0 == 0 && s1 != 0)
		return -1;
	else if (*s0 == s1)
		return s0 - _start;//std::distance
	else
		return _StrChr(s0 + 1, s1, _start);
}

int StrChr(const char * s0, char s1) {
	return _StrChr(s0, s1, s0);
}

int StrChr2(const char * s0, char s1, const char*_orig = nullptr) {
	if (_orig == nullptr)
		return StrChr2(s0, s1, s0);

	if (*s0 == 0 && s1 != 0)
		return -1;
	else if (*s0 == s1)
		return s0 - _orig;//std::distance
	else
		return StrChr2(s0 + 1, s1, _orig);
}



bool contains(const char * s0, const char * s1) {
	int len = strlen(s0);
	return s1 >= s0 && s1 <= s0 + len;
}

bool startsWith(const char * s0, const char * s1) {
	if (*s0 == 0 && *s1 != 0)
		return false;
	if (*s1 == 0)
		return true;
	if (*s0 != *s1)
		return false;
	else
		return startsWith(s0 + 1, s1 + 1);
}

const char * StrStr(const char * s0, const char * s1) {
	if (startsWith(s0, s1))
		return s0;
	else
		if (*s0 == 0) {
			if (*s1 == 0)
				return s0;
			else 
				return nullptr;
		}
		else
			return StrStr(s0 + 1, s1);
}

void assert(bool b) {
	if (!b) throw std::exception();
}

int main(){
	printf("test int array : %d \n", IntArray::TEST);

	assert(StrStr("animaux", "maux") !=nullptr);
	const char* animaux = "licorne";
	const char*ani = "lic";
	assert(startsWith("animaux", "ani") == true);
	assert(startsWith(animaux, ani) == true);

	assert(startsWith("", "ani") == false);

	int libStrCmp = strcmp("","");
	assert(StrCmp("", "") == 0);
	assert(StrCmp("sapin", "sapin") == 0);
	assert(StrCmp("a", "b") == 1);
	assert(StrCmp("b", "a") == -1);
	char dst[256];
	memset(dst, 0, 256);

	StrCat(dst, "sapin1");
	StrCat(dst, "sapin2");

	StrCat(dst, "");

	assert( StrCmp(dst, "sapin1sapin2") == 0);

	char trash[200];
	memset(trash, 0xcd, 200);
	strcpy_s(trash, 200,"sapin");

	StrCat(trash, "licorne");

	int res = StrChr("calimero", 'a');

	assert(StrStr("ux", "chasseur")== nullptr);
	assert(StrStr("animaux", "chasseur")==nullptr);
	assert(StrStr("animaux", "ux") != nullptr);
	assert(StrStr("animaux", "ani")!=nullptr);
	

	auto nameTab0 = "tab0";
	auto nameTab1 = "tab1";
	{
		//variable dynamique
		//auto montableau = new IntArray(16);
		//IntArray * montableau = new IntArray(16);

		//variable locale
		IntArray tab0 = IntArray(16, nameTab0);
	}

	{
		IntArray * tab1 = new IntArray(16, nameTab1);

		//si on ne fait pas ca, la mémoire a fuit, on a perdu la trace de tab1
		//delete(tab1);
		assert( tab1->ensure(8) == false);
		assert( tab1->ensure(32) == true );
		delete(tab1);
		tab1 = nullptr;
	}

	{
		IntArray t(16);
		t.set(0, 8);
		t.set(3, 66);
		for (int i = 0; i < t.getLength(); ++i) {
			printf("t[%d]: %d\n", i,t[i]);
			//printf("t[%d]: %d\n", i, t(i));
		}

		t[5] = 8;
		printf("t[%d]: %d\n", 5, t[5]);

	}

	{
		IntArray t(6);
		for (int i = 0; i < 6; i++) {
			t.set(i, i*i);
		}
		t.push_back(5);
		t.push_front(2);
	}

	{
		IntArray t(0);
		t.push_back(5);
		t.push_front(2);
	}
	{
		IntArray t(4);
		for (int i = 0; i < 4; i++) {
			t.set(i, i*i);
		}
		t.insert(2, 1024);
		int i = 0;
	}

	{
		IntArray t(4);
		assert(t.getLength() == 0);
		t.insert(0, 77);
		assert(t.getLength() == 1);

		t.insert(5, 0);
		assert(t.getLength() == 6);

		t.insert(2, 0);
		assert(t.getLength() == 7);

		int i = 0;
	}

	{
		IntArray t(1);
		t.set(8, 66);
		assert(t.getLength() == 9);
	}

	{
		IntArray t(1);
		t.set(8, 66);
		assert(t.getLength() == 9);
		t.set(2, 67);
		assert(t.getLength() == 9);
	}

	{
		IntArray t(1);
		assert(t.searchPosition(0) == 0);
		t.set(0, 5);
		t.set(1, 6);
		t.set(2, 10);
		assert(t.getLength() == 3);
		assert(t.searchPosition(1) == 0);
		assert(t.searchPosition(5) == 0);
		assert(t.searchPosition(6) == 1);
		assert(t.searchPosition(8) == 2);
		assert(t.searchPosition(12) == 3);
		int i = 0;
	}

	{
		IntArray t;
		t[0] = 68;
		t[1] = 69;
		t[2] = 70;

		t.remove(t[1]);
		assert(t[0] == 68);
		assert(t[1] == 70);
		int i = 0;
	}

	{
		IntArray t;
		t.insert(t.searchPosition(10),10);
		t.insert(t.searchPosition(0),0);
		t.insert(t.searchPosition(8),8);
		assert(t[0] == 0);
		assert(t[1] == 8);
		assert(t[2] == 10);
	}//on se retrouve avec un tableau trié

	{
		IntArray t;
		t.fillWithRandom(16);

		clock_t cnt = clock();
		srand( cnt );
		for (int i = 0; i < t.getLength(); ++i) {
			int val = std::rand() % 100;
			t.set(i, val);
		}

		int  i = 0;
	}

	{
		IntArray t;
		t.fillWithRandom(16);
		t.sort();

		int o = 0;
	}


	//ici montableau a été détruit

    std::cout << "Hello World!\n"; 
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Conseils pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.


