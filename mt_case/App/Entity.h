#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

using namespace sf;
class Entity {
public:
	
	sf::Shape * spr = nullptr;
	Vector2f speed;

	Entity(sf::Shape * spr) {
		this->spr = spr;
	}

	void setSpeed(Vector2f sp) {
		speed = sp;
	}

	bool willCollide(Vector2f previewSpeed, Entity * other, b2Manifold * res) {

		sf::FloatRect oBounds = spr->getGlobalBounds();
		sf::FloatRect mBounds = spr->getGlobalBounds();

		b2Manifold coll;
		b2EdgeShape e;

		b2Vec2 src(mBounds.getPosition().x, mBounds.getPosition().y);
		e.Set(src, src + b2Vec2(previewSpeed.x,previewSpeed.y) );

		b2Transform trA;
		trA.SetIdentity();

		b2PolygonShape polyB;

		polyB.SetAsBox(oBounds.getSize().x,oBounds.getSize().y);
		b2Transform trB;
		trB.SetIdentity();
		trB.p.Set(oBounds.left, oBounds.top);

		if (res == nullptr)
			res = &coll;

		b2CollideEdgeAndPolygon(res, &e, trA, &polyB, trB);

		return res->pointCount > 0;
	}

};