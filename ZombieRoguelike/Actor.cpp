#include "stdafx.h"
#include "main.h"
#include <math.h>
#include <algorithm>

Actor::Actor(int x, int y, char ch, const char *name,
	const TCODColor &col) :
	x(x), y(y), ch(ch), col(col), name(name),
	blocks(true), fovOnly(true), describer(NULL), attacker(NULL), destructible(NULL), 
	ai(NULL), pickable(NULL), usable(NULL), container(NULL) {
}

Actor::Actor(const Actor &other) :
	x(other.x), y(other.y), ch(other.getAscii()), col(other.getColor()), 
	name(other.getName()), blocks(other.blocks), fovOnly(other.fovOnly) {

	if (other.describer) {
		describer = other.describer->copy();
	}

	if (other.attacker) {
		attacker = new Attacker(other.attacker->power);
	}

	if (other.destructible) {
		destructible = new Destructible(other.destructible->maxHp, other.destructible->defense, other.destructible->corpseName, other.destructible->xp);
	}
	
	if (other.ai) {
		ai = other.ai->copy();
	}

	if (other.pickable) {
		pickable = new Pickable();
	}
		
	if (other.usable) {
		usable = other.usable->copy();
	}

	if (other.container) {
		container = new Container(other.container->size);
	}
}

Actor::Actor(Actor && other) : Actor() {
	using std::swap;
	swap(*this, other);
}

Actor & Actor::operator=(Actor other) { 
	using std::swap; 
	swap(x, other.x); 
	return *this; 
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

const char * Actor::getName() const {
	return name;
}

const char Actor::getAscii() const {
	return ch;
}

void Actor::setAscii(char ch) {
	this->ch = ch;
}

void Actor::render() const {
	TCODConsole::root->setChar(x, y, ch);
	TCODConsole::root->setCharForeground(x, y, col);  

	if (ai) {
		ai->render(this);
	}
}

const TCODColor Actor::getColor() const {
	return col;
}

void Actor::setColor(TCODColor col) {
	this->col = col;
}

void Actor::update() {
	if (ai) 
		ai->update(this);
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x - cx;
	int dy = y - cy;

	assert(SQRTF(dx, dy) == sqrtf(dx*dx + dy*dy));
	return SQRTF(dx, dy);
}

std::ostream & operator<<(std::ostream &stream, const Actor &actor) {
	return stream << "Actor - " << actor.name <<" is represent as '" << actor.ch << "'" << " at " << actor.x << "," << actor.y;
}