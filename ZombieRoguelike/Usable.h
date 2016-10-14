#pragma once
class Usable
{
public:
	virtual ~Usable() = default;

	virtual Usable* copy() = 0;
	virtual bool use(Actor *owner, Actor *wearer) = 0;
};

class Healer : public Usable {
public:
	float amount; // how many hp

	Healer(float amount);
	Healer* copy() override;;
	bool use(Actor *owner, Actor *wearer) override;
	//void load(TCODZip &zip);
	//void save(TCODZip &zip);
};

