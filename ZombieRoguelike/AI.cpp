#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <string>
#include "Main.h"

using std::string;
using namespace bt1;

// how many turns the monster chases the player
// after losing his sight
static const int TRACKING_TURNS = 3;

MonsterAi::MonsterAi() : moveCount(0) {
}

Ai* MonsterAi::copy() {
	return new MonsterAi(*this);
}

void MonsterAi::update(Actor *owner) {
	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}
	if (engine.map->isInFov(owner->x, owner->y)) {
		// we can see the player. move towards him
		moveCount = TRACKING_TURNS;
	}
	else {
		moveCount--;
	}
	if (moveCount > 0) {
		moveOrAttack(owner, engine.player->x, engine.player->y);
	}
}

void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	int dx = targetx - owner->x;
	int dy = targety - owner->y;
	int stepdx = (dx > 0 ? 1 : -1);
	int stepdy = (dy > 0 ? 1 : -1);
	float distance = sqrtf(dx*dx + dy*dy);
	if (distance >= 2) {
		dx = (int)(round(dx / distance));
		dy = (int)(round(dy / distance));
		if (engine.map->canWalk(owner->x + dx, owner->y + dy)) {
			owner->x += dx;
			owner->y += dy;
		}
		else if (engine.map->canWalk(owner->x + stepdx, owner->y)) {
			owner->x += stepdx;
		}
		else if (engine.map->canWalk(owner->x, owner->y + stepdy)) {
			owner->y += stepdy;
		}
	}
	else if (owner->attacker) {
		owner->attacker->attack(owner, engine.player);
	}
}

DuplicateAi::DuplicateAi():duplicate(false) {

}

Ai* DuplicateAi::copy() {
	return new DuplicateAi(*this);
}

void DuplicateAi::update(Actor *owner) {
	if (!duplicate && engine.map->isInFov(owner->x, owner->y)) {
		// we can see the player. duplicate another actor
		duplicate = true;

		Actor *twins = new Actor(*owner);
		engine.actors.push(twins);		
	}

	MonsterAi::update(owner);
}

TeleportAi::TeleportAi() : teleport(false), wait(false) {

}

Ai* TeleportAi::copy() {
	return new TeleportAi(*this);
}

void TeleportAi::update(Actor *owner) {
	if (!teleport && engine.map->isInFov(owner->x, owner->y)) {
		if (!wait) {
			// for render and better user experience, wait for a turn
			wait = true;
		}
		else {
			// we can see the player. teleport actor
			teleport = true;
			owner->x += (engine.player->x - owner->x) / 2;
			owner->y += (engine.player->y - owner->y) / 2;
		}
	}

	MonsterAi::update(owner);
}

//
//ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns, Ai *oldAi)
//	: nbTurns(nbTurns), oldAi(oldAi) {
//}
//
//void ConfusedMonsterAi::update(Actor *owner) {
//	if (!owner->destructible || !owner->destructible->isDead()) {
//		TCODRandom *rng = TCODRandom::getInstance();
//		int dx = rng->getInt(-1, 1);
//		int dy = rng->getInt(-1, 1);
//		if (dx != 0 || dy != 0) {
//			int destx = owner->x + dx;
//			int desty = owner->y + dy;
//			if (engine.map->canWalk(destx, desty)) {
//				owner->x = destx;
//				owner->y = desty;
//			}
//			else {
//				Actor *actor = engine.getActor(destx, desty);
//				if (actor) {
//					owner->attacker->attack(owner, actor);
//				}
//			}
//		}
//	}
//	nbTurns--;
//	if (nbTurns == 0) {
//		owner->ai = oldAi;
//		delete this;
//	}
//}

const int dir = 4; // number of possible directions to go at any position
// if dir==4
static int dxx[dir]={1, 0, -1, 0};
static int dyy[dir]={0, 1, 0, -1};
				   // if dir==8
//static int dxx[dir] = { 1, 1, 0, -1, -1, -1, 0, 1 };
//static int dyy[dir] = { 0, 1, 1, 1, 0, -1, -1, -1 };

PlayerAi::PlayerAi() : xpLevel(1), dx(0), dy(0) {
	con = new TCODConsole(engine.screenWidth, engine.screenHeight);
}

Ai* PlayerAi::copy() {
	return new PlayerAi(*this);
}

int PlayerAi::getNextLevelXp() {
	return LEVEL_UP_BASE + xpLevel*LEVEL_UP_FACTOR;
}

void PlayerAi::update(Actor *owner) {
	
	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}

	checkExperience(owner);

	//int dx = 0, dy = 0;

	if (engine.mouse.rbutton_pressed) {
		if (engine.map->isExplored(engine.mouse.cx, engine.mouse.cy)) {
			engine.gui->message(TCODColor::lightGrey, "You clicked (%d, %d)", engine.mouse.cx, engine.mouse.cy);
			path = engine.map->pathFind(owner->x, owner->y, engine.mouse.cx, engine.mouse.cy);
		}

		//		// highlight the possible range
		//		for (int cx = 0; cx < map->width; cx++) {
		//			for (int cy = 0; cy < map->height; cy++) {
		//				if (map->isInFov(cx, cy)
		//					&& (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)) {
		//					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
		//					col = col * 1.2f;
		//					TCODConsole::root->setCharBackground(cx, cy, col);
		//				}
		//			}
		//		}
		//		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
		//		if (map->isInFov(mouse.cx, mouse.cy)
		//			&& (maxRange == 0 || player->getDistance(mouse.cx, mouse.cy) <= maxRange)) {
		//			TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);
		//			if (mouse.lbutton_pressed) {
		//				*x = mouse.cx;
		//				*y = mouse.cy;
		//				return true;
		//			}
		//		}
		//		if (mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
		//			return false;
		//		}
		//		TCODConsole::flush();
	}
	else {
		switch (engine.lastKey.vk) {
		case TCODK_UP: case TCODK_KP8: dy = -1; break;
		case TCODK_DOWN: case TCODK_KP2: dy = 1; break;
		case TCODK_LEFT: case TCODK_KP4: dx = -1; break;
		case TCODK_RIGHT: case TCODK_KP6: dx = 1; break;
		case TCODK_KP7: dy = dx = -1; break;
		case TCODK_KP9: dy = -1; dx = 1; break;
		case TCODK_KP1: dx = -1; dy = 1; break;
		case TCODK_KP3: dx = dy = 1; break;
		case TCODK_KP5: engine.gameStatus = Engine::NEW_TURN; break;
		case TCODK_CHAR: handleActionKey(owner, engine.lastKey.c); break;
		default:break;
		}

		if (dx != 0 || dy != 0) {
			path.clear();
		}
	}

	if (dx != 0 || dy != 0) {
		engine.gameStatus = Engine::NEW_TURN;
		if (moveOrAttack(owner, owner->x + dx, owner->y + dy)) {
			engine.map->computeFov();
		}

		dx = 0;
		dy = 0;
	}
}

void PlayerAi::checkExperience(Actor *owner) {
	int levelUpXp = getNextLevelXp();

	if (owner->destructible->xp >= levelUpXp) {
		xpLevel++;
		owner->destructible->xp -= levelUpXp;
		owner->destructible->heal();
		engine.gui->message(TCODColor::yellow, "You reached level %d! \nYou feel healthy and your battle skills grow stronger!", xpLevel);

		engine.gui->menu.clear();
		engine.gui->menu.addItem(Menu::CONSTITUTION, "Constitution (+20HP)");
		engine.gui->menu.addItem(Menu::STRENGTH, "Strength (+1 attack)");
		engine.gui->menu.addItem(Menu::AGILITY, "Agility (+1 defense)");

		Menu::MenuItemCode menuItem = engine.gui->menu.pick(Menu::PAUSE);
		switch (menuItem) {
		case Menu::CONSTITUTION:
			owner->destructible->maxHp += 20;
			owner->destructible->hp += 20;
			break;
		case Menu::STRENGTH:
			owner->attacker->power += 1;
			break;
		case Menu::AGILITY:
			owner->destructible->defense += 1;
			break;
		default:break;
		}
	}
}

void PlayerAi::render(const Actor *owner) const {
	if (!path.empty()) {
		char c; int j;
		int x = owner->x;
		int y = owner->y;

		for (int i = 0; i<path.length(); i++)
		{
			c = path.at(i);
			j = atoi(&c);
			x = x + dxx[j];
			y = y + dyy[j];

			TCODConsole::root->setCharBackground(x, y, TCODColor::grey);

			//if (i == 0) {
			//	printf("(%d, %d)", x, y);
			//}
			//else if (i == path.length() - 1) {
			//	printf("\n");
			//	//TCODConsole::root->clear();
			//	//con->clear();
			//}
			//else {
			//	printf(" -> (%d, %d)", x, y);
			//}
		}
	}
}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	if (engine.map->isWall(targetx, targety)) 
		return false;
	
	// look for living actors to attack
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->destructible && !actor->destructible->isDead()
			&& actor->x == targetx && actor->y == targety) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}

	// nothing will block the player, move player
	owner->x = targetx;
	owner->y = targety;

	// each move will take some health point
	owner->destructible->takeDamage(owner, owner->destructible->defense + 1);

	// move to next level
	if (engine.stairs->x == owner->x && engine.stairs->y == owner->y) {
		engine.nextLevel();
		return false;
	}

	// look for corpses or items
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;

		// check position
		if (actor->x != targetx || actor->y != targety) {
			continue;
		}

		if (actor->destructible && actor->destructible->isDead()) {
			engine.gui->message(TCODColor::lightGrey, "There's a %s here.", actor->destructible->corpseName);
		} 
		else if (actor->pickable) {
			// pick up item
			if (actor->pickable->pick(actor, owner)) {
				iterator = engine.actors.remove(iterator);
			}
			else {
				engine.gui->message(TCODColor::lightGrey, "Your package is full, cannot pick %s..", actor->getName());
			}
		} 
		else if (actor->usable) {
			// use item
			if (actor->usable->use(actor, owner)) {
				delete actor;
				iterator = engine.actors.remove(iterator);
			}
			else {
				Healer *healer = dynamic_cast<Healer *>(actor->usable);

				if (healer) { // dynamic cast successfully
					engine.gui->message(TCODColor::lightGrey, "Your hp is full!! You cannot eat %s right now!!", actor->getName());
				}
				else {
					engine.gui->message(TCODColor::lightGrey, "You cannot use %s right now..", actor->getName());
				}
			}
		}
	}

	return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii) {
	switch (ascii) {
	case 'd': // drop item 
	{
		Actor *actor = choseFromInventory(owner);
		if (actor) {
			actor->pickable->drop(actor, owner);
			engine.gameStatus = Engine::NEW_TURN;
		}
	}
	break;
	case 'i': // display inventory
	{
		Actor *actor = choseFromInventory(owner);
		if (actor) {
			if (actor->usable) {
				actor->usable->use(actor, owner);

				// remove from inventory
				owner->container->remove(actor);
				delete actor;

				engine.gameStatus = Engine::NEW_TURN;
			}
			else {
				engine.gui->message(TCODColor::red, "%s cannot be used..", actor->getName());
			}
		}
	}
	break;
	}
}

Actor *PlayerAi::choseFromInventory(Actor *owner) {
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);

	// display the inventory frame
	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true,
		TCOD_BKGND_DEFAULT, "inventory");

	// display the items with their keyboard shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int y = 1;

	std::for_each(owner->container->inventory.begin(), owner->container->inventory.end(), [&y, &shortcut](Actor *actor) {
		con.print(2, y++, "(%c) %s", shortcut++, actor->getName());
	});

	//for (Actor *actor : owner->container->inventory) {
	//	con.print(2, y, "(%c) %s", shortcut, actor->getName());
	//	y++;
	//	shortcut++;
	//}

	// blit the inventory console on the root console
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
		TCODConsole::root, engine.screenWidth / 2 - INVENTORY_WIDTH / 2,
		engine.screenHeight / 2 - INVENTORY_HEIGHT / 2);
	TCODConsole::flush();

	// wait for a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if (key.vk == TCODK_CHAR) {
		int actorIndex = key.c - 'a';
		if (actorIndex >= 0 && actorIndex < owner->container->inventory.size()) {
			return owner->container->inventory.at(actorIndex);
		}
	}
	return NULL;
}
