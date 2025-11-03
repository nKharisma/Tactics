#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

#include "../json/atlas_parser.h"
#include "map.h"
#include "isometric_converter.h"
#include "draw.h"
#include "unit.h"
#include "player_unit.h"
#include "ai_enemy.h"
#include "cursor.h"
#include "aStar.h"
#include "UI.h"
#include "turnmanager.h"

enum ActionState {
	Move,
	Attack,
	Idle
};

#define MAP_OFFSET_X ((800 - (60 * 15)) / 2)

class Game {
public:
	Game();
	~Game();
	
	bool Initialize();
	void InitializeUI();
	void Run();
	void HandleEvents();
	void HandleAITurn(Unit* aiUnit);
	Unit* FindNearestPlayerUnit(Unit* aiUnit);
	void MoveAIUnitTowardTarget(Unit* aiUnit, Unit* target);
	void Update(double deltaTime);
	void Render();
	void RenderHUD();
	void Cleanup();
	void UpdateCursorFromMouse(const SDL_Event& event, Cursor& cursor, const IsometricConverter& isoConverter);
	void HandleMouseButtonDown(const SDL_Event& event);
	
private: 
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	TTF_Font* m_font;
	bool m_isRunning;

    AtlasParser m_atlasParser;
    AtlasParser m_unitAtlasParser;
    AtlasParser m_shadowAtlasParser;
    AtlasParser m_mapIndicatorsParser;
	IsometricConverter m_isometricConverter;
	Map m_map;
	Cursor m_cursor;
	int mouseX;
	int mouseY;

	Draw_Game m_reference;
	PlayerUnit m_player;
	AIEnemy m_enemy;
	aStar m_pathfinder;
	TurnManager m_turnManager;
	Unit* selectedUnit = nullptr;
	std::vector<Unit*> m_units;
	UI m_ui;
	ActionState m_actionState = ActionState::Idle;
};

#endif