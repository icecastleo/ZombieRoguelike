#include <stdio.h>

class Ai : public Persistent {
public:
	virtual Ai* copy() = 0;
	virtual void update(Actor *owner) = 0;
	static Ai *create(TCODZip &zip);
protected:
	enum AiType {
		MONSTER, CONFUSED_MONSTER, PLAYER
	};
};

class MonsterAi : public Ai {
public:
	MonsterAi();
	Ai* copy() override;
	void update(Actor *owner) override;
	void load(TCODZip &zip);
	void save(TCODZip &zip);
protected:
	int moveCount;

	void moveOrAttack(Actor *owner, int targetx, int targety);
};

class DuplicateAi : public MonsterAi {
public:
	DuplicateAi();
	Ai* copy() override;
	void update(Actor *owner) override;
protected:
	bool duplicate;
};

class PlayerAi : public Ai {
public:
	int xpLevel;
	PlayerAi();
	Ai* copy() override;
	int getNextLevelXp();
	void update(Actor *owner) override;
	void load(TCODZip &zip);
	void save(TCODZip &zip);
protected:
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	void handleActionKey(Actor *owner, int ascii);
	Actor *choseFromInventory(Actor *owner);
};