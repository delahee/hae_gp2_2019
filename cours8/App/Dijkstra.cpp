#include "Dijkstra.hpp"

void Dijkstra::init(const std::vector<Vector2f>& _graph, const Vector2f & _start)
{
	//copier le graphe depuis l'argument dans la variable membre
	//initialiser les distances à l'infini pour tout les sommets
	//initialier la distance du sommet de départ à 0
	G = _graph;
	start = _start;
	for (int i =0; i < G.size();i++) 
	{
		Vector2f vec = G[i];
		distance[vec] = 9999999999;
	}
	distance[_start] = 0;

	int i = 0;
}

Vector2f Dijkstra::findMin(std::vector<Vector2f>& queue)
{
	double mini = INFINITY;
	Vector2f sommet = Vector2f(-1, -1);
	for (const Vector2f& vec : queue)
	{
		if (distance[vec] < mini)
		{
			mini = distance[vec];
			sommet = vec;
		}
	}
	return sommet;
}

double Dijkstra::weight(Vector2f a, Vector2f b)
{
	Vector2f dif = b - a;
	return sqrt((dif.x*dif.x + dif.y*dif.y));
}

void Dijkstra::updateDist(Vector2f a, Vector2f b)
{
}

void Dijkstra::compute()
{
}

bool Dijkstra::findPath(std::vector<Vector2f>& result)
{
	return false;
}
