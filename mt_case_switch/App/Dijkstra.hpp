#pragma once

#include "Lib.hpp"
#include <algorithm>
#include <unordered_map>

namespace std {
	template <>
	struct hash<Vector2i>
	{
		std::size_t operator()(const Vector2i& k) const
		{
			return (unsigned int)(k.x << 16) | ((unsigned int)k.y);
		}
	};
}

class Dijkstra {
public:

	std::vector<Vector2i>						vertices;
	std::unordered_map<Vector2i, Vector2i>		pred;
	std::unordered_map<Vector2i, float>			distance;
	Vector2i start;

	void	compute(Vector2i start, const std::vector<Vector2i> & verts);
	void	retrievePath( Vector2i end, std::vector<Vector2i> list );

	int		findMin(std::vector<Vector2i> & queue);
	void	updateDistance(Vector2i a, Vector2i b);
};