#pragma once

struct Tile {
	bool explored; // has the player already seen this tile ?
	Tile() : explored(false) {}
};

class Map : public Persistent {
public:
	int width, height;

	Map(int width, int height);
	Map(const Map& otherMap);
	Map& operator=(const Map& otherMap);
	~Map();
	bool isWall(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	bool canWalk(int x, int y) const;
	void computeFov();
	void render() const;

	void load(TCODZip &zip);
	void save(TCODZip &zip);
	void init(bool withActors);
protected:
	Tile *tiles;
	TCODMap *map;
	long seed;
	TCODRandom *rng;
	friend class BspListener;

	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors);
	virtual void addMonster(int x, int y, char c);
	void addItem(int x, int y);

private:
	void addMonster(int x, int y);
};