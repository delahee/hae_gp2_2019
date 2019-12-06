#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;

class Entity {
public:

	int CX_W = 16;
	int CX_H = 16;

	int cx = 0;
	int cy = 0;

	float rx = 0.5;
	float ry = 0.5;

	float dx = 0.0f;
	float dy = 0.0f;


	float pixelX = 0.0;
	float pixelY = 0.0;

	sf::Shape * spr;

	Entity( sf::Shape * spr ) {
		this->spr = spr;
	}

	void update(double dt) {
		syncCoord();
	}

	void syncCoord() {
		pixelX = cx * CX_W + rx * CX_W;
		spr->setPosition(pixelX, pixelY );
	}

	void draw(sf::RenderWindow & win) {
		win.draw(*spr);
	}
};
