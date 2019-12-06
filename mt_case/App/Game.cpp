#include "Game.hpp"

void Game::update(double dt)
{

}

void Game::draw(RenderWindow & win)
{
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
}
