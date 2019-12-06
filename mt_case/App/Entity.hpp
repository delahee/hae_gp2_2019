#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;

class Entity {
public:
	int CELL_WIDTH = 16; // taille de chaque case width

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

	void setPos(float crx, float cry) {
		rx += crx;
		ry += crx;
	}

	void setPosPixel(float pixelX, float pixelY);

	void update(double dt);


	void syncCoord() {
		pixelX = cx * CELL_WIDTH + rx * CELL_WIDTH;
		pixelY = cy * CELL_WIDTH + ry * CELL_WIDTH;
		spr->setPosition(pixelX, pixelY );
	}

	void draw(sf::RenderWindow & win);
};
