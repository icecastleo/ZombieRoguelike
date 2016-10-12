#pragma once
class Usable
{
public:
	virtual bool use(Actor *owner, Actor *wearer) = 0;
};

class Healer : public Usable {
public:
	float amount; // how many hp

	Healer(float amount);
	bool use(Actor *owner, Actor *wearer);
	//void load(TCODZip &zip);
	//void save(TCODZip &zip);
};

