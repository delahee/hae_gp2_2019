#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

using namespace std;
using namespace sf;

class Entity {
public:
	const static int CELL_WIDTH = 16; // taille de chaque case width

	int cx = 0;
	int cy = 0;

	float rx = 0.5f;
	float ry = 0.5f;

	float dx = 0.0f;
	float dy = 0.0f;

	float pixelX = 0.0;
	float pixelY = 0.0;

	bool applyGravity = false;

	sf::Shape * spr;

	static void doNothing(Entity*e) {};

	function <void(Entity*)> initState;
	function <void(Entity*)> updateState;

	Entity( sf::Shape * spr ) {
		this->spr = spr;
		changeToIdle();
	}

	void setPos(float crx, float cry) {
		rx += crx;
		ry += crx;
	}

	void changeToIdle();
	void changeToWalk();
	void changeToCover();
	void changeToRun();

	bool detectWall();
	void setPosPixel(float pixelX, float pixelY);

	void updateControls();
	void updateMove();
	void update(double dt);

	//void dropParticles();

	void syncCoord() {
		pixelX = cx * CELL_WIDTH + rx * CELL_WIDTH;
		pixelY = cy * CELL_WIDTH + ry * CELL_WIDTH;
		spr->setPosition(pixelX, pixelY );
	}

	void draw(sf::RenderWindow & win);

	bool		willCollide(int cx, int cy);

	float lat_acc = 0.075;
	float max_lat_speed = 0.75;
	int		coverTimer = 0;
private:
};
