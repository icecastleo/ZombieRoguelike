#include "stdafx.h"
#include "main.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

class BspListener : public ITCODBspCallback {
private:
	Map &map; // a map to dig
	int roomNum; // room number
	int lastx, lasty; // center of the last room
public:
	BspListener(Map &map) : map(map), roomNum(0) {}
	bool visitNode(TCODBsp *node, void *userData) {
		if (node->isLeaf()) {
			int x, y, w, h;
			
			w = map.rng->getInt(ROOM_MIN_SIZE, node->w - 2);
			h = map.rng->getInt(ROOM_MIN_SIZE, node->h - 2);
			x = map.rng->getInt(node->x + 1, node->x + node->w - w - 1);
			y = map.rng->getInt(node->y + 1, node->y + node->h - h - 1);

			bool withActors = (bool)userData;
			
			// dig a room
			map.createRoom(roomNum == 0, x, y, x + w - 1, y + h - 1, withActors);
			
			if (roomNum != 0) {
				// dig a corridor from last room
				map.dig(lastx, lasty, x + w / 2, lasty);
				map.dig(x + w / 2, lasty, x + w / 2, y + h / 2);
			}

			lastx = x + w / 2;
			lasty = y + h / 2;
			roomNum++;
		}
		return true;
	}
};

Map::Map(int width, int height)
	: width(width), height(height) {
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

void Map::init(bool withActors) {
	rng = new TCODRandom(seed, TCOD_RNG_CMWC);
	tiles = new Tile[width*height];
	map = new TCODMap(width, height);
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, (void *)withActors);
}

Map::~Map() {
	delete[] tiles;
	delete map;
}

void Map::dig(int x1, int y1, int x2, int y2) {
	if (x2 < x1) {
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if (y2 < y1) {
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}
	for (int tilex = x1; tilex <= x2; tilex++) {
		for (int tiley = y1; tiley <= y2; tiley++) {
			map->setProperties(tilex, tiley, true, true);
		}
	}
}

void Map::addMonster(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	if (rng->getInt(0, 100) < 80) {
		// create an orc
		Actor *zombie = new Actor(x, y, 'z', "zombie",
			TCODColor::desaturatedGreen);
		zombie->describer = new MonsterDescriber();
		zombie->destructible = new MonsterDestructible(10, 0, "zombie debris", 35);
		zombie->attacker = new Attacker(3);
		zombie->ai = new MonsterAi();
		engine.actors.push(zombie);
	}
	else {
		// create a troll
		Actor *fleshy = new Actor(x, y, 'Z', "fleshy zombie ",
			TCODColor::darkerGreen);
		fleshy->describer = new MonsterDescriber();
		fleshy->destructible = new MonsterDestructible(16, 1, "fleshy zombie debris", 100);
		fleshy->attacker = new Attacker(4);
		fleshy->ai = new MonsterAi();
		engine.actors.push(fleshy);
	}
}

void Map::addItem(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);
	if (dice < 50) {
		// create a bread
		Actor *rice = new Actor(x, y, '!', "rice",
			TCODColor::violet);
		rice->blocks = false;
		rice->usable = new Healer(12);
		engine.actors.push(rice);
	}
	else if (dice < 75) {
		Actor *bread = new Actor(x, y, '!', "bread",
			TCODColor::violet);
		bread->blocks = false;
		bread->pickable = new Pickable();
		bread->usable = new Healer(8);
		engine.actors.push(bread);
	}

	//else if (dice < 70 + 10) {
	//	// create a scroll of lightning bolt 
	//	Actor *scrollOfLightningBolt = new Actor(x, y, '#', "scroll of lightning bolt",
	//		TCODColor::lightYellow);
	//	scrollOfLightningBolt->blocks = false;
	//	scrollOfLightningBolt->pickable = new LightningBolt(5, 20);
	//	engine.actors.push(scrollOfLightningBolt);
	//}
	//else if (dice < 70 + 10 + 10) {
	//	// create a scroll of fireball
	//	Actor *scrollOfFireball = new Actor(x, y, '#', "scroll of fireball",
	//		TCODColor::lightYellow);
	//	scrollOfFireball->blocks = false;
	//	scrollOfFireball->pickable = new Fireball(3, 12);
	//	engine.actors.push(scrollOfFireball);
	//}
	//else {
	//	// create a scroll of confusion
	//	Actor *scrollOfConfusion = new Actor(x, y, '#', "scroll of confusion",
	//		TCODColor::lightYellow);
	//	scrollOfConfusion->blocks = false;
	//	scrollOfConfusion->pickable = new Confuser(10, 8);
	//	engine.actors.push(scrollOfConfusion);
	//}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors) {
	dig(x1, y1, x2, y2);
	if (!withActors) {
		return;
	}
	if (first) {
		// put the player in the first room
		engine.player->x = (x1 + x2) / 2;
		engine.player->y = (y1 + y2) / 2;
	}
	else {
		TCODRandom *rng = TCODRandom::getInstance();
		// add monsters
		int nbMonsters = rng->getInt(0, MAX_ROOM_MONSTERS);
		while (nbMonsters > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y)) {
				addMonster(x, y);
			}
			nbMonsters--;
		}
		// add items
		int nbItems = rng->getInt(0, MAX_ROOM_ITEMS);
		while (nbItems > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y)) {
				addItem(x, y);
			}
			nbItems--;
		}
		// set stairs position
		engine.stairs->x = (x1 + x2) / 2;
		engine.stairs->y = (y1 + y2) / 2;
	}
}

bool Map::isWall(int x, int y) const {
	return !map->isWalkable(x, y);
}

bool Map::canWalk(int x, int y) const {
	if (isWall(x, y)) {
		// this is a wall
		return false;
	}
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->blocks && actor->x == x && actor->y == y) {
			// there is a blocking actor here. cannot walk
			return false;
		}
	}
	return true;
}

bool Map::isExplored(int x, int y) const {
	return tiles[x + y*width].explored;
}

bool Map::isInFov(int x, int y) const {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return false;
	}
	if (map->isInFov(x, y)) {
		tiles[x + y*width].explored = true;
		return true;
	}
	return false;
}

void Map::computeFov() {
	map->computeFov(engine.player->x, engine.player->y,
		engine.fovRadius);
}

void Map::render() const {
	static const TCODColor darkWall(0, 0, 100);
	static const TCODColor darkGround(50, 50, 150);
	static const TCODColor lightWall(130, 110, 50);
	static const TCODColor lightGround(200, 180, 50);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (isInFov(x, y)) {
				TCODConsole::root->setCharBackground(x, y,
					isWall(x, y) ? lightWall : lightGround);
			}
			else if (isExplored(x, y)) {
				TCODConsole::root->setCharBackground(x, y,
					isWall(x, y) ? darkWall : darkGround);
			}
		}
	}
}