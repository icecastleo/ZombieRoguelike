#include "stdafx.h"
#include <stdio.h>
#include "main.h"

Attacker::Attacker(float power) : power(power) {
}

void Attacker::attack(Actor *owner, Actor *target) {
	if (target->destructible && !target->destructible->isDead()) {
		if (power - target->destructible->defense > 0) {
			engine.gui->message(owner == engine.player ? TCODColor::red : TCODColor::lightGrey,
				"%s attacks %s for %g hit points.", owner->getName(), target->getName(),
				power - target->destructible->defense);
		}
		else {
			engine.gui->message(TCODColor::lightGrey,
				"%s attacks %s but it has no effect!", owner->getName(), target->getName());
		}
		target->destructible->takeDamage(target, power);
	}
	else {
		engine.gui->message(TCODColor::lightGrey,
			"%s attacks %s in vain.", owner->getName(), target->getName());
	}
}