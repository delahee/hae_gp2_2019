#include "Entity.hpp"
#include "Game.hpp"

using namespace std;
using namespace sf;

void Entity::setPosPixel(float pixelX, float pixelY) {
	cx = (int)pixelX / CELL_WIDTH;
	cy = (int)pixelY / CELL_WIDTH;

	rx = (pixelX - (cx*CELL_WIDTH)) / CELL_WIDTH;//reste de cx pour aller a rx
	ry = (pixelY - (cy*CELL_WIDTH)) / CELL_WIDTH;

	syncCoord();
}

void Entity::update(double dt) {
	rx += dx;

	//maintain cell coherency
	if (dx > 0) {
		while (rx > 1) {
			rx--;
			cx++;
		}
	}
	else if (dx < 0) {
		while (rx < 0) {
			rx++;
			cx--;
		}
	}

	ry += dy;

	//apply friction
	dx *= 0.92;

	//cap dx min val to avoid sliding
	if (abs(dx) < 0.05) dx = 0;

	syncCoord();
}

void Entity::draw(sf::RenderWindow & win) {

	sf::Text coords;
	coords.setFont(*Game::me->fnt);
	coords.setPosition(pixelX + 20, pixelY - 20);
	coords.setFillColor(sf::Color::Red);
	coords.setString(
		std::to_string(cx) + "," + std::to_string(cy)
		+ " " + std::to_string(rx) + ","+std::to_string(ry));

	win.draw(*spr);
	win.draw(coords);
}
