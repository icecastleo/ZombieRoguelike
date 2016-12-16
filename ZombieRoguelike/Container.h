#pragma once
#include <vector>

class Container : public Persistent {
public:
	int size; // maximum number of actors. 0=unlimited
	std::vector<Actor *> inventory;

	Container(int size);
	~Container();
	bool add(Actor *actor);
	void remove(Actor *actor);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};
