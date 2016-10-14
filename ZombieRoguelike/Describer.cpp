#include "stdafx.h"
#include "main.h"

Describer* PlayerDescriber::copy() {
	return new PlayerDescriber(*this);
}

char *PlayerDescriber::getDescription(Actor *owner) {
	return "unnamed hero, trying to survive in this game..";
}

Describer* MonsterDescriber::copy() {
	return new MonsterDescriber(*this);
}

char *MonsterDescriber::getDescription(Actor *owner) {	
	return "just a normal zombie..";
}
