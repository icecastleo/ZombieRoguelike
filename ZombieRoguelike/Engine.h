#pragma once
#include <vector>

class EngineObserver
{
public:
	virtual void updateLevel(int level) = 0;
};

class Gui;

class Engine {
public:
	enum GameStatus {
		STARTUP,
		IDLE,
		NEW_TURN,
		VICTORY,
		DEFEAT
	} gameStatus;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
	TCODList<Actor *> actors;
	Actor *player;
	Actor *stairs;
	Map *map;
	int fovRadius;
	int screenWidth;
	int screenHeight;
	Gui *gui;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void update();
	void render();
	void sendToBack(Actor *actor);
	Actor *getActor(int x, int y) const;
	Actor *getClosestMonster(int x, int y, float range) const;
	//bool pickATile(int *x, int *y, float maxRange = 0.0f);
	void nextLevel();
	void load(bool pause = false);
	void save();
	void init();
	void term();

	void setLevel(int level) {
		m_level = level;
		notifyAll();
	}

	void registerObserver(EngineObserver *obs) {
		m_level_views.push_back(obs);
		notify(obs);
	}

protected:
	int m_level;
	std::vector<EngineObserver *> m_level_views;

	void notifyAll() {
		for (int i = 0; i < m_level_views.size(); ++i)
			notify(m_level_views[i]);
	}

	void notify(EngineObserver *obs) {
		obs->updateLevel(m_level);
	}
};

extern Engine engine;