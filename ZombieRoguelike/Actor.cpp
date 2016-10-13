#include "stdafx.h"
#include "main.h"
#include <math.h>

Actor::Actor(int x, int y, int ch, const char *name,
	const TCODColor &col) :
	x(x), y(y), ch(ch), col(col), name(name),
	blocks(true), fovOnly(true), describer(NULL), attacker(NULL), destructible(NULL), 
	ai(NULL), pickable(NULL), usable(NULL), container(NULL) {
}

Actor::~Actor() {
	if (describer) delete describer;
	if (attacker) delete attacker;
	if (destructible) delete destructible;
	if (ai) delete ai;
	if (pickable) delete pickable;
	if (usable) delete usable;
	if (container) delete container;
}

void Actor::render() const {
	TCODConsole::root->setChar(x, y, ch);
	TCODConsole::root->setCharForeground(x, y, col);
}

void Actor::update() {
	if (ai) ai->update(this);
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x - cx;
	int dy = y - cy;
	return sqrtf(dx*dx + dy*dy);
}

std::ostream & operator<<(std::ostream &stream, const Actor &actor) {
	return stream << "Actor - " << actor.name <<" is represent as '" << actor.ch << "'" << " at " << actor.x << "," << actor.y;
}
