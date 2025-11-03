#ifndef PLAYER_UNIT_H
#define PLAYER_UNIT_H
#include "unit.h"
#include <string>

class PlayerUnit : public Unit {
	public:
		PlayerUnit(const std::string& name, int gridx, int gridY);
};

#endif // PLAYER_UNIT_H