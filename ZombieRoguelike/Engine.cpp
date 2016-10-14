#include "stdafx.h"
#include "main.h"
#include <math.h>
#include <sstream>

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP),
player(NULL), map(NULL), fovRadius(10),
screenWidth(screenWidth), screenHeight(screenHeight), level(1) {
	TCODConsole::initRoot(screenWidth, screenHeight, "Zombie Roguelike", false);
	gui = new Gui();
}

void Engine::init() {
	player = new Actor(40, 25, '@', "player", TCODColor::white);
	player->describer = new PlayerDescriber();
	player->destructible = new PlayerDestructible(START_HP, 2, "your cadaver");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push(player);
	stairs = new Actor(0, 0, '>', "stairs", TCODColor::white);
	stairs->blocks = false;
	stairs->fovOnly = false;
	actors.push(stairs);

	map = new Map(80, 43);
	map->init(true);

#ifdef _WIN64
	engine.gui->message(TCODColor::yellow, "This is a 64-bit program!!");
#else
	engine.gui->message(TCODColor::yellow, "This is a 32-bit program!!");
#endif

	gui->message(TCODColor::red,
		"Welcome stranger!\nPrepare to escape from endless zombies in the dungeons.");

	// print out player
	std::stringstream ss;
	ss << *player;
	engine.gui->message(TCODColor::lightGrey, ss.str().c_str());

	gameStatus = STARTUP;
}

Engine::~Engine() {
	term();
	delete gui;
}

void Engine::term() {
	actors.clearAndDelete();
	if (map) delete map;
	gui->clear();
}

void Engine::update() {
	if (gameStatus == STARTUP) 
		map->computeFov();

	gameStatus = IDLE;

	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
	if (lastKey.vk == TCODK_ESCAPE) {
		save();
		load(true);
	}
	player->update();
	
	if (gameStatus == NEW_TURN) {
		for (Actor **iterator = actors.begin(); iterator != actors.end();
			iterator++) {
			Actor *actor = *iterator;
			if (actor != player) {
				actor->update();
			}
		}
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	// draw the map
	map->render();
	// draw the actors
	for (Actor **iterator = actors.begin();
		iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor != player
			&& ((!actor->fovOnly && map->isExplored(actor->x, actor->y))
				|| map->isInFov(actor->x, actor->y))) {
			actor->render();
		}
	}
	player->render();
	// show the player's stats
	gui->render();
}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}

Actor *Engine::getActor(int x, int y) const {
	for (Actor **iterator = actors.begin();
		iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->x == x && actor->y == y && actor->destructible
			&& !actor->destructible->isDead()) {
			return actor;
		}
	}
	return NULL;
}

Actor *Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closest = NULL;
	float bestDistance = 1E6f;
	for (Actor **iterator = actors.begin();
		iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor != player && actor->destructible
			&& !actor->destructible->isDead()) {
			float distance = actor->getDistance(x, y);
			if (distance < bestDistance && (distance <= range || range == 0.0f)) {
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

bool Engine::pickATile(int *x, int *y, float maxRange) {
	while (!TCODConsole::isWindowClosed()) {
		render();
		// highlight the possible range
		for (int cx = 0; cx < map->width; cx++) {
			for (int cy = 0; cy < map->height; cy++) {
				if (map->isInFov(cx, cy)
					&& (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
					col = col * 1.2f;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
		if (map->isInFov(mouse.cx, mouse.cy)
			&& (maxRange == 0 || player->getDistance(mouse.cx, mouse.cy) <= maxRange)) {
			TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);
			if (mouse.lbutton_pressed) {
				*x = mouse.cx;
				*y = mouse.cy;
				return true;
			}
		}
		if (mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}

void Engine::nextLevel() {
	level++;
	gui->message(TCODColor::lightViolet, "You take a moment to rest, and recover your strength.");
	player->destructible->heal(player->destructible->maxHp / 2);
	gui->message(TCODColor::red, "After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");
	delete map;
	// delete all actors but player and stairs
	for (Actor **it = actors.begin(); it != actors.end(); it++) {
		if (*it != player && *it != stairs) {
			delete *it;
			it = actors.remove(it);
		}
	}
	// create a new map
	Map *map1 = new Map(100, 43);
	Map *map2 = new Map(150, 43);

	map2 = map1;

	if (level < 2)
	{
		map = new Map(80, 43);
		map->init(true);
		gameStatus = STARTUP;
	}
	else
	{
		map = new Map(*map2);
		map->init(true);
		gameStatus = STARTUP;
	}
}