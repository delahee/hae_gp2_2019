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

	makePlatforms();
}

static unsigned int palette[]= {
	0xFA8A7Fff,
	0xCC75D9ff,
	0x918DF0ff,
	0x75C2D9ff,
	0x88FCB4ff
};

static int palIdx = 2;
void Game::makePlatforms() {
	int cScreenWidth = 1280 / Entity::CELL_WIDTH;
	int cScreenHeight = 720 / Entity::CELL_WIDTH;

	platforms.push_back( Vector3i( 8, cScreenHeight-1 , palette[palIdx] ));
	platforms.push_back( Vector3i(8, cScreenHeight-2, palette[palIdx] ));
}

void Game::update(double dt){
	for (auto it = evec.begin(); it != evec.end();) {
		Entity * ent = *it;
		ent->update(dt);
		it++;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) palIdx = 0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2)) palIdx = 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3)) palIdx = 2;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4)) palIdx = 3;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5)) palIdx = 4;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F6)) platforms.clear();

	wasPressed[sf::Keyboard::Up] = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
}

void Game::draw(RenderWindow & win)
{
	for (auto it = evec.begin(); it != evec.end();) {
		Entity * ent = *it;
		ent->draw(win);
		it++;
	}

	auto CW = Entity::CELL_WIDTH;
	RectangleShape cell;
	
	cell.setSize(Vector2f(CW, CW));
	for (Vector3i & pl : platforms) {
		cell.setFillColor(sf::Color(pl.z));
		cell.setPosition(pl.x * CW, pl.y * CW);
		win.draw(cell);
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

bool Game::willCollide(Entity * end, int cx, int cy)
{
	int cScreenWidth = 1280 / Entity::CELL_WIDTH;
	int cScreenHeight = 720/ Entity::CELL_WIDTH;

	if (cx < 0) return true;
	else if (cx >= cScreenWidth  ) return true;
	
	if (cy < 0) return true;
	else if (cy >= cScreenHeight ) return true;

	for (Vector3i & cell : platforms)
		if (cell.x == cx && cell.y == cy)
			return true;

	return false;
}

void Game::togglePlatform(int cx, int cy)
{
	for (auto it = platforms.begin(); it != platforms.end(); it++) {
		Vector3i & pl = *it;
		if (pl.x == cx && pl.y == cy) {
			platforms.erase(it);
			return;
		}
	}
	platforms.push_back(Vector3i(cx, cy, palette[palIdx]));
}
