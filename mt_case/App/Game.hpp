#pragma once


#include <iostream>
#include <SFML/Graphics.hpp>
#include <direct.h>
#include <functional>

#include "Lib.hpp"
#include "Particle.h"
#include "Action.hpp"
#include <Box2D/Box2D.h>

using namespace sf;

class Game {
public:
	std::vector< Particle * > pvec;

	void update(double dt);
	void draw(RenderWindow & win);
};
