#pragma once
#include "libtcod.hpp"
#include <string>
#include <list>

#include "catch.hpp"

#define START_HP 100
#define LEVEL_UP_BASE 200
#define LEVEL_UP_FACTOR 150

#define SQRTF(a, b) sqrtf((a)*(a) + (b)*(b))

class Actor;
#include "Persistent.h"
#include "Destructible.h"
#include "Describer.h"
#include "Attacker.h"
#include "Ai.h"
#include "Pickable.h"
#include "Usable.h"
#include "Container.h"
#include "Actor.h"

#include "Map.h"
#include "Gui.h"
#include "Engine.h"

//#define NDEBUG 
#include <assert.h>