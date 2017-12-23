#include <stdio.h>
#include <string>

class Ai : public Persistent {
public:
	virtual Ai* copy() = 0;
	virtual void update(Actor *owner) = 0;
	static Ai *create(TCODZip &zip);

	virtual void render(const Actor *owner) const {};
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

class TeleportAi : public MonsterAi {
public:
	TeleportAi();
	Ai* copy() override;
	void update(Actor *owner) override;
protected:
	bool teleport;
	bool wait;
};

class PlayerAi : public Ai {
public:
	int xpLevel;
	int dx, dy;
	bt1::Behavior *behavior;
	
	PlayerAi();
	~PlayerAi();

	Ai* copy() override;
	bt1::Behavior * getPlayerBehavior();
	int getNextLevelXp();
	void update(Actor *owner) override;
	void load(TCODZip &zip);
	void save(TCODZip &zip);

	std::string path;
	void render(const Actor *owner) const;

protected:
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	void handleActionKey(Actor *owner, int ascii);
	Actor *choseFromInventory(Actor *owner);
	void checkLevelUp(Actor *owner);
	TCODConsole *con;
};