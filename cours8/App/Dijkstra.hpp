#pragma once

#include "Lib.hpp"
#include <algorithm>
#include <unordered_map>

using namespace std;

//permet de faire une map de Vector2i
namespace std {
	template <>
	struct hash<Vector2f>
	{
		std::size_t operator()(const Vector2f& k) const
		{
			return (unsigned int)(((int)k.x) << 16) | ((unsigned int)k.y);
		}
	};

	//table [n][m]
	//=
	//[n][n][n][n][n][n][n] ... mfois 
	//table ** = [ [n] [n] [n]... m fois ]

	// 0.5, 0 => 0 <<16 | 0 => 0
	// 1    0			| (1<<16) | 0 = 65535 | 0 =>
	// 1	1			=> 65535 | 1 = 65536

	//0 - 65535				//tous les y pour x = 0,
	//65535 - 65535 *2		//tous les y pour x = 1,
	//65535*2 - 65535 *3	//tous les y pour x = 2,
}

class Dijkstra {
public:
	/*
	0,0 ----- 1,0 ----- 2,0
	|			|		|
	|			|		|
	|			|		|
	0,1 ----- 1,1 ----- 2,1
	|		   |        |
	|		   |		|
	|		   |		|
	0,2 ----- 1,2 ----- 2,2
	...
	0,y ----- 1,y ----- x,y 
	*/
	std::vector<Vector2f>					G;
	Vector2f								start;
	std::unordered_map<Vector2f, double>	distance;
	std::unordered_map<Vector2f, Vector2f>	pred;
	bool									computed = false;
	/*
	Initialisation(G,sdeb)
	 pour chaque point s de G faire
	    d[s] := infini             // on initialise les sommets autres que sdeb à infini 
	 fin pour
	d[sdeb] : = 0                  // la distance au sommet de départ sdeb est nulle 
	*/
	//passer tous les sommets qu'on veut examiner
	void init(const std::vector<Vector2f> & _graph, const Vector2f & _start );
	
	/*
	Trouve_min(Q)
	mini := infini
	sommet := -1
	pour chaque sommet s de Q
	   si d[s] < mini
	   alors
	       mini := d[s]
	       sommet := s
	renvoyer sommet
	*/
	Vector2f findMin( std::vector<Vector2f> & queue );
	int findMin2(std::vector<Vector2f> & queue);

	//Poids(a,b)
	double weight(Vector2f a, Vector2f b);

	/*
	maj_distances(s1, s2)
	 si d[s2] > d[s1] + Poids(s1, s2)      // Si la distance de sdeb à s2 est plus grande que 
	                                      // celle de sdeb à S1 plus celle de S1 à S2 
	    alors
	        d[s2] : = d[s1] + Poids(s1, s2) // On prend ce nouveau chemin qui est plus court 
	        prédécesseur[s2] : = s1        // En notant par où on passe 
	*/
	void updateDist(Vector2f a, Vector2f b);

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
	void compute(std::vector<Vector2f> G, Vector2f sdeb);

	/*
	A = suite vide
	s := sfin
	tant que s != sdeb faire
	A = A + s                 // on ajoute s à la suite A
	s = prédécesseur[s]       // on continue de suivre le chemin
	fin tant que
	*/
	//returns true if a path was found
	bool findPath(std::vector<Vector2f> & result,const Vector2f & end);
};