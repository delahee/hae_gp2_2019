#pragma once

#include "Particle.h"

using namespace sf;
class FadingParticle : public Particle {
public:
	int fadeStart = 20;

	FadingParticle(sf::Shape * s) : Particle(s) {

	}

	virtual void update() {
		Particle::update();
		if (life < fadeStart) {
			auto sc = spr->getScale();
			spr->setScale(Vector2f(sc.x * 0.95, sc.y * 0.95));
		}
	}

};
