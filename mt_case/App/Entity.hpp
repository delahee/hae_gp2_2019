#pragma once

#include <SFML/Graphics.hpp>

using namespace sf;

class Entity {
public:

	sf::Shape * spr;

	Entity( sf::Shape * spr ) {
		this->spr = spr;
	}

	void update(double dt) {

	}

	void draw(sf::RenderWindow & win) {
		win.draw(win);
	}
};
