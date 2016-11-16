#include "stdafx.h"
#include "main.h"
//#include <queue>
//using std::priority_queue;

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

const static int MAX_WIDTH = 100; // horizontal size of the map
const static int MAX_HEIGHT = 100; // vertical size size of the map
static int closed_nodes_map[MAX_WIDTH][MAX_HEIGHT]; // map of closed (tried-out) nodes
static int open_nodes_map[MAX_WIDTH][MAX_HEIGHT]; // map of open (not-yet-tried) nodes
static int dir_map[MAX_WIDTH][MAX_HEIGHT]; // map of directions

const int dir = 4; // number of possible directions to go at any position

// if dir==4
static int dx[dir] = { 1, 0, -1, 0 };
static int dy[dir] = { 0, 1, 0, -1 };

// if dir==8
//static int dx[dir] = { 1, 1, 0, -1, -1, -1, 0, 1 };
//static int dy[dir] = { 0, 1, 1, 1, 0, -1, -1, -1 };

class node {
	// current position
	int xPos;
	int yPos;
	// total distance already travelled to reach the node
	int level;
	// priority=level+remaining distance estimate
	int priority;  // smaller: higher priority

public:
	node() = default;

	node(int xp, int yp, int d, int p) {
		xPos = xp; yPos = yp; level = d; priority = p;
	}

	int getxPos() const { return xPos; }
	int getyPos() const { return yPos; }
	int getLevel() const { return level; }
	int getPriority() const { return priority; }

	void updatePriority(const int & xDest, const int & yDest) {
		priority = level + estimate(xDest, yDest) * 10; //A*
	}

	// give better priority to going strait instead of diagonally
	// i: direction 
	void nextLevel(const int & i) {
		level += (dir == 8 ? (i % 2 == 0 ? 10 : 14) : 10);
	}

	// Estimation function for the remaining distance to the goal.
	const int & estimate(const int & xDest, const int & yDest) const {
		static int xd, yd, d;
		xd = xDest - xPos;
		yd = yDest - yPos;

		// Euclidian Distance
		//d = static_cast<int>(sqrt(xd*xd + yd*yd));

		// Manhattan distance
		d=abs(xd)+abs(yd);

		// Chebyshev distance
		//d=max(abs(xd), abs(yd));

		return(d);
	}
};

template <class T>
class C_Vector
{
public:
	typedef T* iterator;

	C_Vector();
	C_Vector(unsigned int size);
	C_Vector(unsigned int size, const T& initial);
	C_Vector(const C_Vector<T>& v);
	~C_Vector();

	unsigned int capacity() const;
	unsigned int size() const;
	bool empty() const;
	iterator begin();
	iterator end();
	T& front();
	T& back();
	void push_back(const T& value);
	void pop_back();

	void reserve(unsigned int capacity);
	void resize(unsigned int size);

	T& operator[](unsigned int index);
	C_Vector<T> & operator=(const C_Vector<T> &);
	void clear();
private:
	unsigned int my_size;
	unsigned int my_capacity;
	T* buffer;
};

template<class T>
C_Vector<T>::C_Vector()
{
	my_capacity = 0;
	my_size = 0;
	buffer = 0;
}

template<class T>
C_Vector<T>::C_Vector(const C_Vector<T> & v)
{
	my_size = v.my_size;
	my_capacity = v.my_capacity;
	buffer = new T[my_size];
	for (unsigned int i = 0; i < my_size; i++)
		buffer[i] = v.buffer[i];
}

template<class T>
C_Vector<T>::C_Vector(unsigned int size)
{
	my_capacity = size;
	my_size = size;
	buffer = new T[size];
}

template<class T>
C_Vector<T>::C_Vector(unsigned int size, const T & initial)
{
	my_size = size;
	my_capacity = size;
	buffer = new T[size];
	for (unsigned int i = 0; i < size; i++)
		buffer[i] = initial;
	//T();
}

template<class T>
C_Vector<T> & C_Vector<T>::operator = (const C_Vector<T> & v)
{
	delete[] buffer;
	my_size = v.my_size;
	my_capacity = v.my_capacity;
	buffer = new T[my_size];
	for (unsigned int i = 0; i < my_size; i++)
		buffer[i] = v.buffer[i];
	return *this;
}

template<class T>
typename C_Vector<T>::iterator C_Vector<T>::begin()
{
	return buffer;
}

template<class T>
typename C_Vector<T>::iterator C_Vector<T>::end()
{
	return buffer + size();
}

template<class T>
T& C_Vector<T>::front()
{
	return buffer[0];
}

template<class T>
T& C_Vector<T>::back()
{
	return buffer[my_size - 1];
}

template<class T>
void C_Vector<T>::push_back(const T & v)
{
	if (my_size >= my_capacity)
		reserve(my_capacity + 5);
	buffer[my_size++] = v;
}

template<class T>
void C_Vector<T>::pop_back()
{
	my_size--;
}

template<class T>
void C_Vector<T>::reserve(unsigned int capacity)
{
	if (buffer == 0)
	{
		my_size = 0;
		my_capacity = 0;
	}
	T * Newbuffer = new T[capacity];
	//assert(Newbuffer);
	unsigned int l_Size = capacity < my_size ? capacity : my_size;
	//copy (buffer, buffer + l_Size, Newbuffer);

	for (unsigned int i = 0; i < l_Size; i++) {
		Newbuffer[i] = buffer[i];
	}

	my_capacity = capacity;
	delete[] buffer;
	buffer = Newbuffer;
}

template<class T>
unsigned int C_Vector<T>::size()const//
{
	return my_size;
}

template<class T>
void C_Vector<T>::resize(unsigned int size)
{
	reserve(size);
	my_size = size;
}

template<class T>
T& C_Vector<T>::operator[](unsigned int index)
{
	return buffer[index];
}

template<class T>
unsigned int C_Vector<T>::capacity()const
{
	return my_capacity;
}

template <class T>
bool C_Vector<T>::empty() const {
	return my_size == 0;
}

template<class T>
C_Vector<T>::~C_Vector()
{
	delete[] buffer;
}

template <class T>
void C_Vector<T>::clear()
{
	my_capacity = 0;
	my_size = 0;
	buffer = 0;
}

template <class T, class Container = C_Vector<T>,
//template <class T, class Container = std::vector<T>,
	class Compare = std::less<T> >
	class priority_queue {
	protected:
		Container c;
		Compare comp;

	public:
		explicit priority_queue(const Container& c_ = Container(),
			const Compare& comp_ = Compare())
			: c(c_), comp(comp_)
		{
			std::make_heap(c.begin(), c.end(), comp);
		}

		bool empty() const { return c.empty(); }
		std::size_t size() const { return c.size(); }

		const T& top() { return c.front(); }

		void push(const T& x) {
			c.push_back(x);
			std::push_heap(c.begin(), c.end(), comp);
		}

		void pop() {
			std::pop_heap(c.begin(), c.end(), comp);
			c.pop_back();
		}
};

// Determine priority (in the priority queue)
bool operator<(const node & a, const node & b) {
	return a.getPriority() > b.getPriority();
}

// A-star algorithm.
// The route returned is a string of direction digits.
string Map::pathFind(const int & xStart, const int & yStart, const int & xFinish, const int & yFinish) {

	static priority_queue<node> pq[2]; // list of open (not-yet-tried) nodes
	static int pqi; // pq index
	static node* n0;
	static node* m0;
	static int i, j, x, y, xdx, ydy;
	static char c;
	pqi = 0;

	// reset the node maps
	for (y = 0; y<MAX_WIDTH; y++)
	{
		for (x = 0; x<MAX_HEIGHT; x++)
		{
			closed_nodes_map[x][y] = 0;
			open_nodes_map[x][y] = 0;
		}
	}

	// create the start node and push into list of open nodes
	n0 = new node(xStart, yStart, 0, 0);
	n0->updatePriority(xFinish, yFinish);
	pq[pqi].push(*n0);
	open_nodes_map[x][y] = n0->getPriority(); // mark it on the open nodes map

	// A* search
	while (!pq[pqi].empty())
	{
		// get the current node w/ the highest priority
		// from the list of open nodes
		n0 = new node(pq[pqi].top().getxPos(), pq[pqi].top().getyPos(),
			pq[pqi].top().getLevel(), pq[pqi].top().getPriority());

		x = n0->getxPos(); y = n0->getyPos();

		pq[pqi].pop(); // remove the node from the open list
		open_nodes_map[x][y] = 0;
		// mark it on the closed nodes map
		closed_nodes_map[x][y] = 1;

		// quit searching when the goal state is reached
		//if((*n0).estimate(xFinish, yFinish) == 0)
		if (x == xFinish && y == yFinish)
		{
			// generate the path from finish to start
			// by following the directions
			string path = "";
			while (!(x == xStart && y == yStart))
			{
				j = dir_map[x][y];
				c = '0' + (j + dir / 2) % dir;
				path = c + path;
				x += dx[j];
				y += dy[j];
			}

			// garbage collection
			delete n0;
			// empty the leftover nodes
			while (!pq[pqi].empty()) pq[pqi].pop();

			// print debug
			int j; char c;
			int x = xStart;
			int y = yStart;

			for (int i = 0; i<path.length(); i++)
			{
				c = path.at(i);
				j = atoi(&c);
				x = x + dx[j];
				y = y + dy[j];
				if (i == 0) {
					printf("(%d, %d)", x, y);
				}
				else if (i == path.length() - 1) {
					printf("\n");
				}
				else {
					printf(" -> (%d, %d)", x, y);
				}
			}

			return path;
		}

		// generate moves (child nodes) in all possible directions
		for (i = 0; i<dir; i++)
		{
			xdx = x + dx[i];
			ydy = y + dy[i];

			if (canWalk(xdx, ydy) && closed_nodes_map[xdx][ydy] == 0)
			{
				// generate a child node
				m0 = new node(xdx, ydy, n0->getLevel(),
					n0->getPriority());
				m0->nextLevel(i);
				m0->updatePriority(xFinish, yFinish);

				// if it is not in the open list then add into that
				if (open_nodes_map[xdx][ydy] == 0)
				{
					open_nodes_map[xdx][ydy] = m0->getPriority();
					pq[pqi].push(*m0);
					// mark its parent node direction
					dir_map[xdx][ydy] = (i + dir / 2) % dir;
				}
				else if (open_nodes_map[xdx][ydy]>m0->getPriority())
				{
					// update the priority info
					open_nodes_map[xdx][ydy] = m0->getPriority();
					// update the parent direction info
					dir_map[xdx][ydy] = (i + dir / 2) % dir;

					// replace the node
					// by emptying one pq to the other one
					// except the node to be replaced will be ignored
					// and the new node will be pushed in instead
					while (!(pq[pqi].top().getxPos() == xdx &&
						pq[pqi].top().getyPos() == ydy))
					{
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pq[pqi].pop(); // remove the wanted node

					// empty the larger size pq to the smaller one
					if (pq[pqi].size()>pq[1 - pqi].size()) pqi = 1 - pqi;
					while (!pq[pqi].empty())
					{
						pq[1 - pqi].push(pq[pqi].top());
						pq[pqi].pop();
					}
					pqi = 1 - pqi;
					pq[pqi].push(*m0); // add the better node instead
				}
				else delete m0; // garbage collection
			}
		}
		delete n0; // garbage collection
	}
	return ""; // no route found
}

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
	: width(width > MAX_WIDTH ? MAX_WIDTH : width), height(height > MAX_HEIGHT ? MAX_HEIGHT : height) {
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

Map::Map(const Map& otherMap)
{
	width = otherMap.width;
	height = otherMap.height;
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

Map& Map::operator=(Map const& otherMap)
{
	if (this != &otherMap)
	{
		width = otherMap.width;
		height = otherMap.height;
		seed = otherMap.seed;
	}
	return *this;
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
	int dice = rng->getInt(0, 100);

	if (dice < 80) {
		addMonster(x, y, 'z');
	}
	else {
		addMonster(x, y, 'Z');
	}
}

Map* Map::nextMap()
{
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);

	if (dice < 40) {
		return new Map(width, height);
	}
	else  {
		return new AdvancedMap(width, height);
	}
}

void Map::addMonster(int x, int y, char c) {
	TCODRandom *rng = TCODRandom::getInstance();
	//int dice = rng->getInt(0, 100);

	Actor *actor;

	switch (c)
	{
	case 'z': 
		// create an zombie
		actor = new Actor(x, y, 'z', "Zombie",
			TCODColor::desaturatedGreen);
		actor->describer = new MonsterDescriber();
		actor->destructible = new MonsterDestructible(10, 0, "zombie debris", 35);
		actor->attacker = new Attacker(3);
		actor->ai = new MonsterAi();
		engine.actors.push(actor);
		break;

	case 'Z': 
		// create a fleshy zombie
		actor = new Actor(x, y, 'Z', "Fleshy Zombie",
			TCODColor::darkerGreen);
		actor->describer = new MonsterDescriber();
		actor->destructible = new MonsterDestructible(16, 1, "fleshy zombie debris", 100);
		actor->attacker = new Attacker(4);
		actor->ai = new MonsterAi();
		engine.actors.push(actor);
		break;
	
	case 'D':
		actor = new Actor(x, y, 'D', "Duplicated Zombie",
			TCODColor::darkerGreen);
		actor->describer = new MonsterDescriber();
		actor->destructible = new MonsterDestructible(12, 0, "Duplicated zombie debris", 75);
		actor->attacker = new Attacker(3);
		actor->ai = new DuplicateAi();
		engine.actors.push(actor);
		break;

	case 'T':
		actor = new Actor(x, y, 'T', "Teleport Zombie",
			TCODColor::darkerGreen);
		actor->describer = new MonsterDescriber();
		actor->destructible = new MonsterDestructible(20, 0, "Teleport zombie debris", 150);
		actor->attacker = new Attacker(5);
		actor->ai = new TeleportAi();
		engine.actors.push(actor);
		break;

	default:
		break;
	}
}

void Map::addItem(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);
	if (dice < 50) {
		// create a bread
		Actor *rice = new Actor(x, y, '*', "rice",
			TCODColor::amber);
		rice->blocks = false;
		rice->usable = new Healer(15);
		engine.actors.push(rice);
	}
	else if (dice < 80) {
		Actor *bread = new Actor(x, y, '!', "bread",
			TCODColor::violet);
		bread->blocks = false;
		bread->pickable = new Pickable();
		bread->usable = new Healer(30);
		engine.actors.push(bread);
	}
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

void AdvancedMap::addMonster(int x, int y)
{
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);

	if (dice < 25) {
		Map::addMonster(x, y, 'z');
	} else if( dice < 50) {
		Map::addMonster(x, y, 'Z');
	} else if (dice < 75) {
		Map::addMonster(x, y, 'D');
	} else {
		Map::addMonster(x, y, 'T');
	}
}

Map* AdvancedMap::nextMap()
{
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);

	if (dice < 40) {
		return new AdvancedMap(width, height);
	} else {
		return new NightmareMap(width, height);
	}
}

void NightmareMap::addMonster(int x, int y)
{
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);

	if (dice < 50) {
		Map::addMonster(x, y, 'D');
	} else {
		Map::addMonster(x, y, 'T');
	}
}

Map* NightmareMap::nextMap()
{
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);

	if (dice < 30) {
		return new AdvancedMap(width, height);
	} else {
		return new NightmareMap(width, height);
	}
}
