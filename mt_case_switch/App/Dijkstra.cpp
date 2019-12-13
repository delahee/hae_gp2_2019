#include "Dijkstra.hpp"



void Dijkstra::compute( Vector2i _start, const std::vector<Vector2i> & _verts )
{
	distance.clear();
	vertices.clear();
	pred.clear();

	vertices = _verts;
	start = _start;

	pred[_start] = _start;
	distance[_start] = 0.0;

	std::vector<Vector2i> queue = vertices;

	while (queue.size()) {
		int curIdx = findMin(queue);
		
		if (curIdx == -1) throw std::exception();

		Vector2i cur = queue[curIdx];

		queue.erase( queue.begin() + curIdx);

		for (int x = cur.x - 1; x <= cur.x + 1; x++) {
			for (int y = cur.y - 1; y <= cur.y + 1; y++) 
			{
				if (x == cur.x && y == cur.y)
					continue;
				updateDistance(cur, Vector2i(x, y));
			}
		}
	}
}

void Dijkstra::retrievePath(Vector2i end, std::vector<Vector2i> list)
{
	list.clear();
	Vector2i cur = end;
	while (cur != start) {
		list.push_back(cur);
		cur = pred[cur];
	}
}

int Dijkstra::findMin(std::vector<Vector2i> & queue)
{
	float mini = 1024*1024;
	Vector2i cur(-1,-1);
	int  idx = -1;
	int i = 0;
	for (Vector2i vtx : queue) {
		if (distance[vtx] < mini) {
			mini = distance[vtx];
			cur = vtx;
			idx = i;
		}
		i++;
	}
	return idx;
}

void Dijkstra::updateDistance(Vector2i a, Vector2i b)
{
	if (distance[b] > distance[a] + Lib::v2iDist(a, b)) {
		distance[b] = distance[a] + Lib::v2iDist(a, b);
		pred[b] = a;
	}
}
