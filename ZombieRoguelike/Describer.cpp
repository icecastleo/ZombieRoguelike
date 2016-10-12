#include "stdafx.h"
#include "main.h"

char *PlayerDescriber::getDescription(Actor *owner)
{
	return "unnamed hero, trying to survive in this game..";
}

char *MonsterDescriber::getDescription(Actor *owner)
{	
	return "monster des";
}
