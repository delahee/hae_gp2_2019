#pragma once


#include <iostream>
#include <SFML/Graphics.hpp>
#include <direct.h>
#include <functional>

#include "Lib.hpp"
#include "Particle.h"
#include "Action.hpp"
#include <Box2D/Box2D.h>

#include "Entity.hpp"

using namespace sf;

class Game {
public:
	sf::Font * fnt;

	static Game * me;

public:
	std::vector< Particle * >	pvec;
	std::vector< Entity * >		evec;
	std::vector<Vector2i>		platforms;

	Game() {

	}

	Entity *				player;

	void init();
	void makePlatforms();
	void update(double dt);
	void draw(RenderWindow & win);
	bool willCollide(Entity*, int cx, int cy);
	void togglePlatform(int cx, int cy);
};
