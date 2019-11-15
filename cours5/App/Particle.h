#pragma once


#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

class Particle {
public:

	sf::Shape * spr;
	Vector2f dir;

	int life = 100;
	bool killed = false;

	std::function<void(Particle*)> bhv;

	Particle(sf::Shape * spr) {
		this->spr = spr;
		dir.y = 1;
	}

	~Particle() {
		delete(spr);
		spr = nullptr;
	}

	void update() {
		if (bhv) {
			bhv(this);
		}
		life--;
		if (life == 0) killed = true;
	}

	void draw(RenderWindow & win) {
		win.draw(*spr);
	}
};
