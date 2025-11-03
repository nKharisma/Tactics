#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <map>
#include <vector>
#include <SDL2/SDL.h>

class AtlasParser;
class TurnManager;
class Map;
class aStar;
class IsometricConverter;
struct renderableObject;

class Unit {
	public:
		Unit(const std::string& name, int health, int attackPower, int defense, int attackRange, int gridX, int gridY);
		virtual ~Unit() = default;
		
		int GetHealth() const { return m_health; }
		int GetAttackPower() const { return m_attackPower; }
		int GetDefense() const { return m_defense; }
		int GetAttackRange() const { return m_attackRange; }
		int GetGridX() const { return m_gridX; }
		int GetGridY() const { return m_gridY; }
		const std::string& GetName() const { return m_name; }

		void Update(double deltaTime, Map& map, aStar& pathfinder);
		void SetPosition(int x, int y, Map& map);
		void UpdateTile(Map& map, SDL_Point nextPoint);
		void MoveTo(int x, int y, Map& map, aStar& pathfinder);
		void Attack(Unit& target, TurnManager* turnManager);
		void TakeDamage(int damage, TurnManager* turnManager);
		void SetAIControlledUnit(bool IsAIControlled) { m_isAIControlled = IsAIControlled; }
		bool IsAIControlled() const { return m_isAIControlled; }
		void HandleDeath();

		renderableObject GetShadowRenderableObject(const AtlasParser* shadowAtlasParser, const IsometricConverter* isoConverter) const;
		renderableObject GetRenderableObject(const AtlasParser* atlasParser, const IsometricConverter* isoConverter) const;
		SDL_Point GetScreenPosition(const IsometricConverter* isoConverter) const;
		
		
		//animation stuff later
		enum class AnimationState {
		Idle,
		Moving,
		Attacking,
		Dead
	};
	
	enum class ActionState {
		Idle,
		Moving,
		Attacking
	};

		void SetAnimationState(AnimationState state);
		void LoadAnimations(const AtlasParser* atlasParser);
		AnimationState GetAnimationState() const {return m_animationState;}

	protected:
		std::string m_name;
		int m_health;
		int m_attackPower;
		int m_defense;
		int m_attackRange;
		int m_gridX;
		int m_gridY;
		double m_attackAnimationTimer = 0.0; 
		double movementTimer = 0.0;
		bool m_isAIControlled = false;
	
	AnimationState m_animationState = AnimationState::Idle;
	struct Animation {
		std::vector<SDL_Rect> frames; 
		int currentFrame = 0;         
		double frameDuration = 0.1;   
		double elapsedTime = 0.0;     
	};
	std::map<AnimationState, Animation> m_animations;
	
	std::vector<SDL_Point> m_path;
};

#endif