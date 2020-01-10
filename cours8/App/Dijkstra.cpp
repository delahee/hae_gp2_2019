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

int Dijkstra::findMin2(std::vector<Vector2f>& queue)
{
	double mini = INFINITY;
	int vtx = -1;
	for (int i = 0; i < queue.size();++i) {
		const Vector2f& vec = queue[i];
		if (distance[vec] < mini) {
			mini = distance[vec];
			vtx = i;
		}
	}
	return vtx;
}

double Dijkstra::weight(Vector2f a, Vector2f b)
{
	Vector2f dif = b - a;
	return sqrt((dif.x*dif.x + dif.y*dif.y));
}

void Dijkstra::updateDist(Vector2f a, Vector2f b)
{
	double Poids = weight(a, b);
	if (distance[b] > distance[a] + Poids)
	{
		distance[b] = distance[a] + Poids;
		pred[b] = a;
	}
}

void Dijkstra::compute(std::vector<Vector2f> G, Vector2f sdeb)
{
	/*
	Dijkstra(G,Poids,sdeb)
	 Initialisation(G,sdeb)
	 Q := ensemble de tous les nœuds
	 tant que Q n'est pas un ensemble vide faire
		   s1 := Trouve_min(Q)
		   Q := Q privé de s1
		   pour chaque nœud s2 voisin de s1 faire
			   maj_distances(s1,s2)
		   fin pour
	 fin tant que
	*/
	init(G, sdeb);
	std::vector<Vector2f> Q = G;
	while (Q.size() > 0) {
		int sai = findMin2(Q);
		Vector2f sa = Q[sai];
		Q.erase(Q.begin() + sai);
		for (const Vector2f & s : Q) {
			if (	(s.x == sa.x + 1 && s.y == sa.y )
				||	(s.x == sa.x - 1 && s.y == sa.y) 
				||	(s.x == sa.x  && s.y == sa.y + 1) 
				||	(s.x == sa.x  && s.y == sa.y - 1))
				updateDist(sa, s);
		}
	}
	computed = true;
}

bool Dijkstra::findPath(std::vector<Vector2f>& result, const Vector2f & end)
{
	result.clear();
	Vector2f cur = end;
	while (cur != start ) {
		result.push_back(cur);
		Vector2f prev = cur;
		cur = pred[cur];
		if (prev == cur)
			return false;
	}
	if (result.size() == 0) {
		return false;
	}
	else {
		result.push_back(start);
		std::reverse(result.begin(), result.end());
		return true;
	}
}
