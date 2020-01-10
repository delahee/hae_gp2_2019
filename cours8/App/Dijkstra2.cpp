#include "Dijkstra2.hpp"
#include "Heap.hpp"

void Dijkstra2::init(const std::vector<Vector2f>& _graph, const Vector2f & _start)
{
	//copier le graphe depuis l'argument dans la variable membre
	//initialiser les distances à l'infini pour tout les sommets
	//initialier la distance du sommet de départ à 0
	G.clear();
	G = _graph;

	start = _start;
	for (int i =0; i < G.size();i++) 
	{
		const Vector2f & vec = G[i];
		distance[vec] = 9999999999;
		GExists[vec] = true;
	}
	distance[start] = 0.0f;

	double d = distance[start];

	int i = 0;
}

Vector2f Dijkstra2::findMin(std::vector<Vector2f>& queue)
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

int Dijkstra2::findMin2(std::vector<Vector2f>& queue)
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

double Dijkstra2::weight(Vector2f a, Vector2f b)
{
	Vector2f dif = b - a;
	return sqrt((dif.x*dif.x + dif.y*dif.y));
}

void Dijkstra2::updateDist(Vector2f a, Vector2f b)
{
	double Poids = weight(a, b);
	if (distance[b] > distance[a] + Poids)
	{
		distance[b] = distance[a] + Poids;
		pred[b] = a;
	}
}


struct Gt {
public:
	static Dijkstra2 * dij;

	bool operator()(const Vector2f & a, const Vector2f & b) const {

		return dij->distance[a] < dij->distance[b];
	}
};

Dijkstra2*Gt::dij = nullptr;

void Dijkstra2::compute(std::vector<Vector2f> G, Vector2f sdeb) {
	init(G, sdeb);
	Heap<Vector2f> Q;

	Q.less = [this](const Vector2f & a, const Vector2f & b) {
		return distance[a] < distance[b];
	};
	
	for(const Vector2f & v : G)
		Q.push(v);

	Gt::dij = this;

	while (Q.size() > 0) {
		int sai = 0;
		Vector2f sa = Q.extractMin();

		for( int y = sa.y-1; y <= sa.y +1; y++){
			for (int x = sa.x - 1; x <= sa.x + 1; x++){
				if (sa.x == x && sa.y == y) continue;

				Vector2f s(x, y);

				if (GExists.find(s) == GExists.end()) continue;
				
				updateDist(sa, s);
				
			}
		}
	}
	computed = true;
}

bool Dijkstra2::findPath(std::vector<Vector2f>& result, const Vector2f & end)
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
