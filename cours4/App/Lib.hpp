#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

using namespace sf;

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
	};

	static Vector2f get(int idx, const std::vector<Vector2f> & points) {
		if (idx < 0)idx = 0;
		if (idx >= (int)points.size()) idx = (int)points.size() - 1;
		return points[idx];
	};

	static sf::Vector2f c2(float i, const std::vector<sf::Vector2f> & points) {
		auto p0 = get((int)(i - 1), points);
		auto p1 = get((int)(i), points);
		auto p2 = get((int)(i + 1), points);
		auto p3 = get((int)(i + 2), points);

		auto t = i - (int)i;
		auto resX = catmull(p0.x, p1.x, p2.x, p3.x, t);
		auto resY = catmull(p0.y, p1.y, p2.y, p3.y, t);
		return Vector2f( resX,resY);
	};

	static Vector2f plot2(float t, const std::vector<Vector2f> & points) {
		return c2(t * (points.size() - 1), points);
	}
};