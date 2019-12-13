#include "Entity.hpp"
#include "Game.hpp"
#include "Particle.h"
#include "FadingParticle.h"

using namespace std;
using namespace sf;

void Entity::changeToIdle() {
	initState = [](Entity*e) {
		e->spr->setFillColor(sf::Color::White);
	};
	updateState = [](Entity * e) {
		e->updateControls();
		e->updateMove();

		if (abs(e->dx) > e->lat_acc || abs(e->dy) > e->lat_acc) {
			e->changeToWalk();
			e->coverTimer = 0;
		}
		else {
			e->coverTimer++;
			if (e->coverTimer >= 60 && e->detectWall()) {
				e->coverTimer = 0;
				e->changeToCover();
			}
		}

	};

	initState(this);
}

void Entity::changeToWalk() {
	initState = [](Entity*e) {
		e->spr->setFillColor(sf::Color::Blue);
	};
	updateState = [](Entity * e) {
		e->updateControls();
		e->updateMove();

		if (abs(e->dx) > 5*e->lat_acc || abs(e->dy) > 5*e->lat_acc)
			e->changeToRun();

		if (e->dx == 0 && e->dy == 0)
			e->changeToIdle();
	};

	initState(this);
}

void Entity::changeToCover() {
	initState = [](Entity*e) {
		e->spr->setFillColor(sf::Color::Magenta);
	};
	updateState = [](Entity*e) {
		e->updateControls();
		e->updateMove();

		if (abs(e->dx) > 0 || abs(e->dy) > 0)
			e->changeToWalk();
	};

	initState(this);
}

void Entity::changeToRun() {
	initState = [](Entity*e) {
		e->spr->setFillColor(sf::Color::Red);
	};
	updateState = [](Entity * e) {
		e->updateControls();
		e->updateMove();

		if (abs(e->dx) > 5 * e->lat_acc || abs(e->dy) > 5 * e->lat_acc) {
			e->changeToRun();
		}

		if (abs(e->dx) < e->lat_acc && abs(e->dy) < e->lat_acc) {
			e->changeToIdle();
		}
	};

	initState(this);
}

bool Entity::detectWall()
{
	bool found = false;
	found |= willCollide(cx+1, cy);
	found |= willCollide(cx-1, cy);
	found |= willCollide(cx, cy+1);
	found |= willCollide(cx, cy-1);

	found |= willCollide(cx+1, cy+1);
	found |= willCollide(cx-1, cy-1);
	found |= willCollide(cx+1, cy-1);
	found |= willCollide(cx-1, cy+1);

	return found;
}

void Entity::setPosPixel(float pixelX, float pixelY) {
	cx = (int)pixelX / CELL_WIDTH;
	cy = (int)pixelY / CELL_WIDTH;

	rx = (pixelX - (cx*CELL_WIDTH)) / CELL_WIDTH;//reste de cx pour aller a rx
	ry = (pixelY - (cy*CELL_WIDTH)) / CELL_WIDTH;

	syncCoord();
}

void Entity::updateControls() {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		dx += lat_acc;
		if (dx > max_lat_speed) dx = max_lat_speed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		dx -= lat_acc;
		if (dx < -max_lat_speed) dx = -max_lat_speed;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		dy -= lat_acc;
		if (dy < -max_lat_speed) dy = -max_lat_speed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		dy += lat_acc;
		if (dy > max_lat_speed) dy = max_lat_speed;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
		setPosPixel(100, 600);
		dx = dy = 0.0f;
	}
}

void Entity::updateMove() {
	rx += dx;

	///////X BLOCK 
	//maintain X cell coherency
	if (dx > 0)
		while (rx > 1) {
			if (!willCollide(cx + 1, cy)) {
				rx--;
				cx++;
			}
			else {
				dx = -dx * 0.8;
				rx = 0.9;
				break;
			}
		}
	else if (dx < 0)
		while (rx < 0) {
			if (!willCollide(cx - 1, cy)) {
				rx++;
				cx--;
			}
			else {
				dx = -dx * 0.8;
				rx = 0.1;
				break;
			}
		}

	//apply friction
	dx *= 0.915f;
	//cap dx min val to avoid sliding
	if (abs(dx) < 0.05) dx = 0;

	///////end bloc x

	ry += dy;
	//maintain Y cell coherency
	if (dy > 0)
		while (ry > 1) {
			if (!willCollide(cx, cy + 1)) {
				ry--;
				cy++;
			}
			else {
				dy = 0;
				ry = 0.99;
				break;
			}
		}
	else if (dy < 0)
		while (ry < 0) {
			ry++;
			cy--;

			if (!willCollide(cx, cy - 1)) {
				ry++;
				cy--;
			}
			else {
				dy = 0;
				ry = 0.1;
			}
		}

	dy *= 0.915f;
	if (abs(dy) < 0.05) dy = 0;

}

void Entity::update(double dt) {
	updateState(this);
	syncCoord();
}

/*
void Entity::dropParticles() {
	int life = 30;
	float sp = Lib::rd() + 0.5;
	auto & g = *Game::me;

	float rdx = Lib::rd() * 6;
	float rdy = Lib::rd() * 6 - 8;
	{
		auto c = new CircleShape(8, 64);
		c->setOrigin(8, 8);
		c->setPosition(pixelX + rdx,pixelY + rdy);
		c->setFillColor(sf::Color(0xE580EB1a));
		auto p = new FadingParticle(c);
		p->life = 30;
		p->speed.y = sp;
		p->bhv = Particle::applySpeed;
		g.pvec.push_back(p);

	}

	{
		auto c = new CircleShape(2, 32);
		c->setOrigin(2, 2);
		c->setPosition(pixelX+rdx, pixelY+rdy);
		c->setFillColor(sf::Color(0xE580EBff));

		auto p = new FadingParticle(c);
		p->life = 30;
		p->speed.y = sp;
		p->bhv = Particle::applySpeed;
		g.pvec.push_back(p);
	}
}
*/

void Entity::draw(sf::RenderWindow & win) {

	sf::Text coords;
	coords.setFont(*Game::me->fnt);
	coords.setPosition(pixelX + 20, pixelY - 20);
	coords.setFillColor(sf::Color::Red);
	coords.setString(
		std::to_string(cx) + "," + std::to_string(cy)
		+ " " + std::to_string(rx) + ","+std::to_string(ry)
		);

	win.draw(*spr);
	win.draw(coords);
}

bool Entity::willCollide(int cx, int cy){
	return Game::me->willCollide(this, cx, cy);
}

