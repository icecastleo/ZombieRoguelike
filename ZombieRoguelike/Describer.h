#pragma once
class Describer
{
public:
	Describer() = default;
	virtual ~Describer() = default;
	virtual Describer* copy() = 0;
	friend char *description(Describer *des, Actor *owner);

protected:
	virtual char *getDescription(Actor *owner) = 0;
};

inline char *description(Describer* des, Actor *owner)
{
	return des->getDescription(owner);
}

class PlayerDescriber : public Describer {
	Describer* copy() override;
	char *getDescription(Actor *owner) override;
};

class MonsterDescriber : public Describer {
	Describer* copy() override;
	char *getDescription(Actor *owner) override;
};

