#include "pch.h"
#include "Lib.hpp"

#include <direct.h>
#include <string>

bool Lib::loadFile(const std::string & path, MemFile & out) {
	MemFile res;
	res.data = nullptr;
	res.size = 0;

	FILE * f = nullptr;
	fopen_s(&f, path.c_str() , "r");
	if (f == nullptr) return false;
	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char * mem = (char*)malloc(fsize);

	fread(mem, 1, fsize, f);
	fclose(f);

	res.data = mem;
	res.size = fsize;

	out = res;
	return true;
}
