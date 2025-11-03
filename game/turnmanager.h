#ifndef TURN_MANAGER_H
#define TURN_MANAGER_H

#include <vector>
#include "unit.h"
#include <SDL2/SDL.h>
#include <memory>
#include <algorithm>

class TurnManager {
public:
	void AddUnit(Unit* unit) {
		m_units.push_back(unit);
	}
	
	void RemoveUnit(Unit* unit) {
		auto it = std::find(m_units.begin(), m_units.end(), unit);
		if (it != m_units.end()) {
			m_units.erase(it);
			if (m_currentUnitIndex >= m_units.size()) {
				m_currentUnitIndex = 0;
			}
		}
	}
	
	void StartTurn() {
		if (m_units.empty()) {
			SDL_Log("No units available to start turn.");
			return;
		}
		
		m_currentUnitIndex = 0;
	}
	
	void NextTurn() {
		if (m_units.empty()) {
		SDL_Log("No units available to proceed to next turn.");
			return;
		}
		
		m_currentUnitIndex = (m_currentUnitIndex + 1) % m_units.size();
		SDL_Log("It's now %s's turn.", m_units[m_currentUnitIndex]->GetName().c_str());
	}
	
	Unit* GetCurrentUnit() {
		if (m_units.empty()) return nullptr;
		return m_units[m_currentUnitIndex];
	}
	
	bool IsCurrentUnit(Unit* unit) const {
		return m_units[m_currentUnitIndex] == unit;
	}

	/*
	bool IsCurrentUnitAI() const {
		if (m_units.empty()) return false;
		return m_units[m_currentUnitIndex]->IsAIControlled();
	}*/
	
private:
	std::vector<Unit*> m_units;
	size_t m_currentUnitIndex = 0;
};

#endif