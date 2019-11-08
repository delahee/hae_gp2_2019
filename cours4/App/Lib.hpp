#pragma once

#include <string>
#include <vector>
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

	static float catmull(float p0, float p1, float p2, float p3, float t) {
		float q = 2.0f * p1;
		float t2 = t * t;

		q += (-p0 + p2) 							* t;
		q += (2.0f*p0 - 5.0f*p1 + 4.0f * p2 - p3) 	* t2;
		q += (-p0 + 3.0f * p1 - 3 * p2 + p3) 		* t2 * t;

		return 0.5f * q;
	}

	static float get(int idx, const std::vector<float> & points) {
		if (idx < 0)idx = 0;
		if (idx >= (int)points.size()) idx = (int)points.size() - 1;
		return points[idx];
	}

	static float c1(float i, const std::vector<float> & points) {
		auto p0 = get((int)(i - 1), points);
		auto p1 = get((int)(i), points);
		auto p2 = get((int)(i + 1), points);
		auto p3 = get((int)(i + 2), points);

		float t = i - (int)i;
		float res = catmull(p0, p1, p2, p3, t);
		return res;
	}

	/**
	* t is [0...1]
	*/
	static float plot(float t, const std::vector<float> & points) {
		return c1(t * (points.size() - 1), points);
	}
};