#include "Game.hpp"
#include "Entity.hpp"
#include <SFML/Graphics.hpp>

Game* Game::me = nullptr;

static bool wasPressed[sf::Keyboard::KeyCount];

void Game::init()
{
	RectangleShape *sh = new RectangleShape(Vector2f(12, 24));
	sh->setFillColor(sf::Color::Magenta);
	sh->setOrigin(6, 24);
	
	Entity * ent = new Entity(sh);
	ent->setPosPixel(100, 600);
	evec.push_back(ent);
	player = ent;
	me = this;

	memset(wasPressed, 0, sizeof(wasPressed));
}

void Game::update(double dt){
	auto lat_acc = 0.075;
	auto max_lat_speed = 0.75;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		player->dx += lat_acc;
		if (player->dx > max_lat_speed) player->dx = max_lat_speed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		player->dx -= lat_acc;
		if (player->dx < -max_lat_speed) player->dx = -max_lat_speed;
	}
	if (	sf::Keyboard::isKeyPressed(sf::Keyboard::Up) 
		&&	!wasPressed[sf::Keyboard::Up]
		&&	player->state != ES_FALLING) {
		player->dy -= 0.35;
		player->applyGravity = true;
		player->state = ES_FALLING;
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ) {
		player->setPosPixel(100,600); 
		player->dx = player->dy = 0.0f;
		player->applyGravity = false;
		player->state = ES_IDLE;
	}

	for (auto it = evec.begin(); it != evec.end();) {
		Entity * ent = *it;
		ent->update(dt);
		it++;
	}

	wasPressed[sf::Keyboard::Up] = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
}

void Game::draw(RenderWindow & win)
{
	for (auto it = evec.begin(); it != evec.end();) {
		Entity * ent = *it;
		ent->draw(win);
		it++;
	}

	for (auto it = pvec.begin(); it != pvec.end();) {
		Particle * p = *it;
		p->update();
		if (p->killed) {
			it = pvec.erase(it);
		}
		else {
			p->draw(win);
			it++;
		}
	}

	RectangleShape line(Vector2f(1000, 1));
	line.setFillColor(sf::Color::Green);
	line.setPosition(100, 600);
	win.draw(line);
}
