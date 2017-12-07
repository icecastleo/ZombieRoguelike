#pragma once
#include "main.h"

using namespace bt1;

class GetItemAction : public Behavior {

protected:
	Engine *engine;
	Actor *player;

public:

	GetItemAction(Engine *engine)
		:engine(engine), player(engine->player) {}

	virtual float calculateUtility() {
		if (player->destructible) {
			return 1.0f - player->destructible->hp / player->destructible->maxHp;
		}

		return 0.0f;
	}

	virtual Status update()
	{
		// look for items
		for (Actor **iterator = engine->actors.begin();
			iterator != engine->actors.end(); iterator++) {
			Actor *actor = *iterator;

			// can not see
			if (!engine->map->isInFov(actor->x, actor->y)) {
				continue;
			}

			if (actor->pickable || actor->usable) {
				
				/*PlayerAi *ai = (PlayerAi*)player->ai;
				
				int dx = 0;
				int dy = 0;

				if (actor->x > player->x) {
					dx = 1;
				}
				else if (actor->x < player->x) {
					dx = -1;
				}
				
				if (actor->y > player->y) {
					dy = 1;
				}
				else if (actor->y < player->y) {
					dy = -1;
				}

				if (dx && !engine->map->isWall(player->x + dx, player->y)) {
					ai->dx = dx;
				}
				else if (dy && !engine->map->isWall(player->x, player->y + dy)) {
					ai->dy = dy;
				}*/

				std::string path = engine->map->pathFind(player->x, player->y, actor->x, actor->y);

				if (path.length() == 0)
					return BH_FAILURE;

				char c = path.at(0);
				int j = atoi(&c);

				PlayerAi *ai = (PlayerAi*)player->ai;
				ai->dx = dxx[j];
				ai->dy = dyy[j];

				return BH_SUCCESS;
			}
		}

		return BH_FAILURE;
	}
};

class KillEnemyAction : public Behavior {

protected:
	Engine *engine;
	Actor *player;

public:

	KillEnemyAction(Engine *engine)
		:engine(engine), player(engine->player) {}

	virtual float calculateUtility() {
		if (player->destructible) {
			return player->destructible->hp / player->destructible->maxHp;
		}

		return 0.0f;
	}

	virtual Status update()
	{
		// look for enemy
		for (Actor **iterator = engine->actors.begin();
			iterator != engine->actors.end(); iterator++) {
			Actor *actor = *iterator;

			if (actor == player)
				continue;

			// can not see
			if (!engine->map->isInFov(actor->x, actor->y)) {
				continue;
			}

			if (actor->destructible && !actor->destructible->isDead()) {
				PlayerAi *ai = (PlayerAi*)player->ai;

				int dx = 0;
				int dy = 0;

				if (actor->x > player->x) {
					dx = 1;
				}
				else if (actor->x < player->x) {
					dx = -1;
				}

				if (actor->y > player->y) {
					dy = 1;
				}
				else if (actor->y < player->y) {
					dy = -1;
				}

				if (dx && !engine->map->isWall(player->x + dx, player->y)) {
					ai->dx = dx;
				}
				else if (dy && !engine->map->isWall(player->x, player->y + dy)) {
					ai->dy = dy;
				}

				return BH_SUCCESS;
			}
		}

		return BH_FAILURE;
	}
};

class HeadToExitAction : public Behavior {

protected:
	Engine *engine;
	Actor *player;

public:

	HeadToExitAction(Engine *engine)
		:engine(engine), player(engine->player) {}

	virtual Status update()
	{
		std::string path = engine->map->pathFind(player->x, player->y, engine->stairs->x, engine->stairs->y);

		if (path.length() == 0)
			return BH_FAILURE;

		char c = path.at(0);
		int j = atoi(&c);
		
		PlayerAi *ai = (PlayerAi*)player->ai;
		ai->dx = dxx[j];
		ai->dy = dyy[j];

		return BH_SUCCESS;
	}
};