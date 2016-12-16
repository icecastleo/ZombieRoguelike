#pragma once
class Usable : public Persistent {
public:
	virtual ~Usable() = default;

	//virtual Usable* copy() = 0;
	virtual Usable* copy() { return nullptr; }
	virtual bool use(Actor *owner, Actor *wearer) { return false; }

	virtual void load(TCODZip &zip) {}
	virtual void save(TCODZip &zip) {}
};

class Healer : public Usable {
public:
	float amount; // how many hp

	Healer(float amount);
	Healer* copy() override;
	bool use(Actor *owner, Actor *wearer) override;
	
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

