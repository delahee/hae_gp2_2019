#pragma once

#include <string>
struct MemFile {
	char *		data;
	int			size;

	void dispose() {
		free(data);
		size = 0;
	}
};

class Lib {
public:
	static bool loadFile(const std::string & path, MemFile & out);
};