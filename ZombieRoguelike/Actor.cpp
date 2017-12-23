#include "stdafx.h"
#include "main.h"
#include <math.h>
#include <algorithm>

using namespace bt1;

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

ActorFactory * ActorFactory::getInstance() {
	static ActorFactory *instance;

	if (!instance)
		instance = new ActorFactory();
	return instance;
}

Actor * ActorFactory::makeMonster(char c) {
	Actor *actor;
	
	switch (c)
	{
	case 'z':
		// create an zombie
		actor = new Actor(0, 0, 'z', "Zombie",
			TCODColor::desaturatedGreen);
		actor->describer = new MonsterDescriber();
		actor->destructible = new MonsterDestructible(10, 0, "zombie debris", 35);
		actor->attacker = new Attacker(3);
		actor->ai = new MonsterAi();
		return actor;

	case 'Z':
		// create a fleshy zombie
		actor = new Actor(0, 0, 'Z', "Fleshy Zombie",
			TCODColor::darkerGreen);
		actor->describer = new MonsterDescriber();
		actor->destructible = new MonsterDestructible(16, 1, "fleshy zombie debris", 100);
		actor->attacker = new Attacker(4);
		actor->ai = new MonsterAi();
		return actor;

	case 'D':
		actor = new Actor(0, 0, 'D', "Duplicated Zombie",
			TCODColor::darkerGreen);
		actor->describer = new MonsterDescriber();
		actor->destructible = new MonsterDestructible(12, 0, "Duplicated zombie debris", 75);
		actor->attacker = new Attacker(3);
		actor->ai = new DuplicateAi();
		return actor;

	case 'T':
		actor = new Actor(0, 0, 'T', "Teleport Zombie",
			TCODColor::darkerGreen);
		actor->describer = new MonsterDescriber();
		actor->destructible = new MonsterDestructible(20, 0, "Teleport zombie debris", 150);
		actor->attacker = new Attacker(5);
		actor->ai = new TeleportAi();
		return actor;

	default:
		return nullptr;
	}
}
