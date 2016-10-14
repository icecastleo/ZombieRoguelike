#include "stdafx.h"
#include "main.h"

Healer::Healer(float amount) : amount(amount) {
}

bool Healer::use(Actor *owner, Actor *wearer) {
	if (wearer->destructible) {
		float amountHealed = wearer->destructible->heal(amount);
		if (amountHealed > 0) {
			engine.gui->message(TCODColor::lightGrey, "%s uses %s to restore %.1f health!", wearer->getName(), owner->getName(), amountHealed);
			return true;
		}
	}
	return false;
}