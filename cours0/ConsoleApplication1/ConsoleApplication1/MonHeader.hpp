#pragma once




/*
typedef struct Vec3 {
	float x;
	float y;
	float z;
} VEC3;
*/
/*
struct LIST;
typedef struct List {
	LIST p;
} LIST;
*/
struct MonEpee; //{}

struct Mabotte {
	MonEpee * x;
};

struct MonEpee {
	Mabotte * x;
};

typedef int MONINT;
typedef int IDENTIFIANT;

class Player {
public:
	IDENTIFIANT keyId;
	IDENTIFIANT Passeportid;
};