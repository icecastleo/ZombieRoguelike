#pragma once

class Actor : public Persistent {
public:
	int x, y; // position on map

	bool blocks; // can we walk on this actor?
	bool fovOnly; // only display when in fov
	
	Describer *describer; // something that can be described
	Attacker *attacker; // something that deals damages
	Destructible *destructible; // something that can be damaged
	Ai *ai; // something self-updating
	bt1::Behavior *behavior;

	Pickable *pickable; // something that can be picked and used
	Usable *usable; // something that can be used
	Container *container; // something that can contain actors

	Actor() = default;
	Actor(int x, int y, char ch, const char *name, const TCODColor &col);
	Actor(const Actor &other);  // copy constructor
	Actor(Actor&& other);
	Actor & operator=(Actor other); // assign operator
	~Actor();

	const char *getName() const;
	const char getAscii() const;
	void setAscii(char ch);

	const TCODColor getColor() const;
	void setColor(TCODColor col);

	void update();
	void render() const;
	float getDistance(int cx, int cy) const;
	void load(TCODZip &zip);
	void save(TCODZip &zip);

	friend std::ostream& operator<<(std::ostream&, const Actor&);
	//friend void swap(Actor& first, Actor& second);

private:
	const char *name; // the actor's name
	char ch; // ascii code
	TCODColor col; // color
};

class ActorFactory {
public:
	static ActorFactory *getInstance();
	Actor *makeMonster(char c);
private:
	ActorFactory() = default;
	ActorFactory(const ActorFactory&) = delete;
	const ActorFactory& operator=(const ActorFactory&) = delete;
	~ActorFactory() = default;
};