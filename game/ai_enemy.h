#ifndef AI_ENEMY_H
#define AI_ENEMY_H

#include "unit.h"
#include <string>

class AIEnemy : public Unit {
	public:
		AIEnemy(const std::string& name, int gridX, int gridY);
};

#endif // AI_ENEMY_H