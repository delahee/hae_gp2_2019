#pragma once

#include <functional>
#include <SFML/Graphics.hpp>

class Action {
public:
	bool destroyed = false;

	virtual void update(double dt) {

	}
};

class Agent {
public:
	std::function<void(double dt)> ac;

	Agent(std::function<void(double dt)> _ac) {
		ac = _ac;
	}

	virtual void update(double dt) {
		ac(dt);
	}
};

class ActionList {
public:
	std::vector<Action*> list;

	void push(Action* ac) {
		list.push_back(ac);
	}

	void update( double dt ) {
		for (auto it = list.begin(); it != list.end(); ) {
			(*it)->update(dt);
			if ((*it)->destroyed) {
				delete(*it);
				it = list.erase(it);
			}
			else {
				it++;
			}
		}
	}
};

class Delay : public Action {
public:
	std::function<void(void)> cbk;
	float durMs;
	
	Delay(std::function<void(void)> ac, float time_ms) {
		durMs = time_ms;
		cbk = ac;
	}

	virtual void update(double dt) {
		durMs -= (float)(dt * 1000.0f);
		if (durMs <= 0.0f && cbk) {
			cbk();
			cbk = nullptr;
			destroyed = true;
		}
	}

};