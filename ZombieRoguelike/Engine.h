#pragma once
#include <vector>

class Gui;
class Engine;

class EngineObserver
{
public:
	virtual void updateEngine(const Engine *engine) = 0;
};

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
	std::unique_ptr<Map> map;
	int fovRadius;
	int screenWidth;
	int screenHeight;
	Gui *gui;

	Engine(int screenWidth, int screenHeight);
	~Engine();
	void update();
	void updateRange(Actor ** begin, Actor ** end);
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

	int getLevel() const {
		return m_level;
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
		obs->updateEngine(this);
	}
};

extern Engine engine;