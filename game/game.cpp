#include "game.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

Game::Game()
	: m_window(nullptr), m_renderer(nullptr), m_isRunning(false), m_isometricConverter(48, 32, 370, 100), 
	m_player("Knight", 6, 9), m_enemy("Goblin", 13, 2)
{
}

Game::~Game()
{
	Cleanup();
}

void Game::InitializeUI() {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

    const int buttonWidth = 100;
    const int buttonHeight = 40;
    const int buttonSpacing = 5; 

    int startX = windowWidth - buttonWidth - 5; 
    int startY = windowHeight - (3 * buttonHeight + 2 * buttonSpacing) - 5;

    m_ui.AddElement(std::make_shared<UIButton>("Move", SDL_Rect{startX, startY, buttonWidth, buttonHeight}, [this]() {
        m_actionState = ActionState::Move;
    }));

    startY += buttonHeight + buttonSpacing; 
    m_ui.AddElement(std::make_shared<UIButton>("Attack", SDL_Rect{startX, startY, buttonWidth, buttonHeight}, [this]() {
        m_actionState = ActionState::Attack;
    }));

    startY += buttonHeight + buttonSpacing; 
    m_ui.AddElement(std::make_shared<UIButton>("Idle", SDL_Rect{startX, startY, buttonWidth, buttonHeight}, [this]() {
        m_actionState = ActionState::Idle;
    }));
}

bool Game::Initialize()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
		return false;
	}
	
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
	
	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		SDL_Log("SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
		SDL_Quit();
		return false;
	}
	
	if(TTF_Init() == -1) {
		SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
		IMG_Quit();
		SDL_Quit();
		return false;
	}
	
	m_font = TTF_OpenFont("fonts/Minecraft.ttf", 16);
	if(!m_font) {
		SDL_Log("Failed to load font! SDL_ttf Error: %s", TTF_GetError());
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return false;
	}
	
	m_window = SDL_CreateWindow(
		"Isometric Game", 
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800, 600,
		SDL_WINDOW_SHOWN
	);
		
	if(m_window == nullptr) {
		SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return false;
	}
	
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(m_renderer == nullptr) {
		SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
		SDL_DestroyWindow(m_window);
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return false;
	}
	
	const std::string atlasJsonPath = "data/Isometric_MedievalFantasy_Tiles.json";
	const std::string atlasImagePath = "images/Isometric_MedievalFantasy_Tiles.png";

	if(!m_atlasParser.LoadAtlas(m_renderer, atlasJsonPath, atlasImagePath)) {
		SDL_Log("Failed to load atlas.");
		Cleanup();
		return false;
	}
	
	const std::string unitJsonPath = "data/IsometricTRPGAssetPack_OutlinedEntities.json";
	const std::string unitImagePath = "images/IsometricTRPGAssetPack_OutlinedEntities.png";
	if(!m_unitAtlasParser.LoadAtlas(m_renderer, unitJsonPath, unitImagePath)) {
		SDL_Log("Failed to load unit atlas.");
		Cleanup();
		return false;
	}
	
	m_player.LoadAnimations(&m_unitAtlasParser);
	m_enemy.LoadAnimations(&m_unitAtlasParser);
	
	const std::string shadowJsonPath = "data/shadow.json";
	const std::string shadowImagePath = "images/shadow.png";
	if(!m_shadowAtlasParser.LoadAtlas(m_renderer, shadowJsonPath, shadowImagePath)) {
		SDL_Log("Failed to load shadow atlas.");
		Cleanup();
		return false;
	}
	
	const std::string cursorJsonPath = "data/TRPGIsometricAssetPack_MapIndicators.json";
	const std::string cursorImagePath = "images/TRPGIsometricAssetPack_MapIndicators.png";
	if(!m_mapIndicatorsParser.LoadAtlas(m_renderer, cursorJsonPath, cursorImagePath)) {
		SDL_Log("Failed to load map indicators atlas.");
		Cleanup();
		return false;
	}
	
	srand(static_cast<unsigned int>(time(0))); // seed the random number generator
	m_map.Initialize("game/15x15map.txt"); // initialize the map with random tiles

	m_player.SetPosition(6, 9, m_map);
	m_enemy.SetPosition(13, 2, m_map);
	
	selectedUnit = &m_player;
	m_enemy.SetAIControlledUnit(true);
	
	m_units.push_back(&m_player);
	m_units.push_back(&m_enemy);
	
	for(auto& unit : m_units) {
		m_turnManager.AddUnit(unit);
	}

	InitializeUI();
	m_turnManager.StartTurn();

	SDL_Log("Game initialized successfully.");
	m_isRunning = true;
	return true;
}

void Game::Run() {
	Uint32 lastTime = SDL_GetTicks();
	SDL_Event event;
	// fixed frame rate loop
	while(m_isRunning) {
		Uint32 currentTime = SDL_GetTicks();
		double deltaTime = (currentTime - lastTime) / 1000.0; 
		lastTime = currentTime;

		Unit* currentUnit = m_turnManager.GetCurrentUnit();
		if(currentUnit && currentUnit->IsAIControlled()) {
			HandleAITurn(currentUnit);
		}

		HandleEvents();
		Update(deltaTime);
		Render();
	}
}

void Game::HandleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            HandleMouseButtonDown(event);
            m_ui.HandleMouseEvent(event.button.x, event.button.y, true); // true indicates a click
        } else if (event.type == SDL_MOUSEMOTION) {
            UpdateCursorFromMouse(event, m_cursor, m_isometricConverter);
            m_ui.HandleMouseEvent(event.motion.x, event.motion.y, false);
        }
    }
}

void Game::UpdateCursorFromMouse(const SDL_Event& event, Cursor& cursor, const IsometricConverter& isoConverter) 
{
    // Fixed: Body is now correctly aligned with the function scope
    SDL_Point tilePos = isoConverter.IsometricToWorld(event.motion.x, event.motion.y);
    
    cursor.x = tilePos.x;
    cursor.y = tilePos.y;
    
    bool cursorInBounds = (cursor.x >= 0 && cursor.x < m_map.GetWidth() &&
                           cursor.y >= 0 && cursor.y < m_map.GetHeight());
    
    if (cursorInBounds) {
        SDL_ShowCursor(SDL_DISABLE);
    } else {
        SDL_ShowCursor(SDL_ENABLE);
    }
}

void Game::HandleMouseButtonDown(const SDL_Event& event) {
    Unit* currentUnit = m_turnManager.GetCurrentUnit();
        
    if (!currentUnit) {
        SDL_Log("No current unit.");
        return;
    }
    
    if (currentUnit == selectedUnit) {
        SDL_Point tilePos = m_isometricConverter.IsometricToWorld(event.button.x, event.button.y);

        m_cursor.x = tilePos.x;
        m_cursor.y = tilePos.y;

        bool cursorInBounds = (m_cursor.x >= 0 && m_cursor.x < m_map.GetWidth() &&
                               m_cursor.y >= 0 && m_cursor.y < m_map.GetHeight());
                               
        if (!cursorInBounds) {
            SDL_Log("Click out of bounds at (%d, %d)", m_cursor.x, m_cursor.y);
            return;
        }
    
        Tile& clickedTile = m_map.GetTile(m_cursor.x, m_cursor.y);
        SDL_Log("Clicked on tile at (%d, %d)", m_cursor.x, m_cursor.y);

        // state machine for actions
        switch (m_actionState) {
            case ActionState::Move:
                if (selectedUnit && clickedTile.isWalkable && !clickedTile.occupyingUnit) {
                    SDL_Log("Moving unit to (%d, %d)", m_cursor.x, m_cursor.y);
                    try {
                        selectedUnit->MoveTo(m_cursor.x, m_cursor.y, m_map, m_pathfinder);
                    } catch (const std::exception& e) {
                        SDL_Log("Exception caught in MoveTo: %s", e.what());
                    }
                } else {
                    SDL_Log("Cannot move unit to (%d, %d)", m_cursor.x, m_cursor.y);
                }
                break;

            case ActionState::Attack:
                if (clickedTile.occupyingUnit && clickedTile.occupyingUnit != selectedUnit) {
                    if (selectedUnit) {
                        selectedUnit->Attack(*clickedTile.occupyingUnit, &m_turnManager);
                    }
                }
                break;

            case ActionState::Idle:
                SDL_Log("Idle action selected. No action taken.");
                break;
        }
        
        m_turnManager.NextTurn();
    }
}

void Game::HandleAITurn(Unit* aiUnit) {
	SDL_Log("%s is taking its turn.", aiUnit->GetName().c_str());
	
	Unit* target = FindNearestPlayerUnit(aiUnit);
    if (target) {
        int distance = abs(target->GetGridX() - aiUnit->GetGridX()) + abs(target->GetGridY() - aiUnit->GetGridY());

        if (distance <= aiUnit->GetAttackRange()) {
            aiUnit->Attack(*target, &m_turnManager);
        } else {
            MoveAIUnitTowardTarget(aiUnit, target);
        }
    }
    m_turnManager.NextTurn();
}

Unit* Game::FindNearestPlayerUnit(Unit* aiUnit) {
	Unit* nearestUnit = nullptr;
	int nearestDistance = std::numeric_limits<int>::max();

	for (Unit* unit : m_units) {
		if (!unit->IsAIControlled()) {
			int distance = abs(unit->GetGridX() - aiUnit->GetGridX()) + abs(unit->GetGridY() - aiUnit->GetGridY());
			if (distance < nearestDistance) {
				nearestDistance = distance;
				nearestUnit = unit;
			}
		}
	}

	return nearestUnit;
}

void Game::MoveAIUnitTowardTarget(Unit* aiUnit, Unit* target) {
	if (!aiUnit || !target) return;

	SDL_Log("%s is moving toward %s.", aiUnit->GetName().c_str(), target->GetName().c_str());

	int startX = aiUnit->GetGridX();
	int startY = aiUnit->GetGridY();
	int targetX = target->GetGridX();
	int targetY = target->GetGridY();

	std::vector<SDL_Point> path = m_pathfinder.FindPath(startX, startY, targetX, targetY, m_map);
	if (path.size() > 1) {
		SDL_Point nextPoint = path[1];
		aiUnit->MoveTo(nextPoint.x, nextPoint.y, m_map, m_pathfinder);
	} else {
		SDL_Log("%s has no path to %s.", aiUnit->GetName().c_str(), target->GetName().c_str());
	}
}

void Game::Update(double deltaTime) {
	// update game logic here
	m_player.Update(deltaTime, m_map, m_pathfinder);
	m_enemy.Update(deltaTime, m_map, m_pathfinder);
	m_reference.Update(deltaTime);
}

void Game::Render() {
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);
    
    SDL_Texture* atlasTexture = m_atlasParser.GetAtlasTexture();
    if(!atlasTexture) {
        SDL_Log("Atlas texture not loaded.");
        return;
    }
    
    m_reference.ClearObjects();
    // first pass: render map tiles/cursor
    for (int x = 0; x < m_map.GetWidth(); ++x) {
        for (int y = 0; y < m_map.GetHeight(); ++y) {
            const Tile& currTile = m_map.GetTile(x, y);
            const sliceInfo* slice = m_atlasParser.GetSliceInfo(currTile.fileName);
            
            if (!slice) {
                std::cerr << "Slice not found for tile: " << currTile.fileName << " at (" << y << ", " << x << ")" << std::endl;
                continue;
            }

            SDL_Point isoPos = m_isometricConverter.WorldToIsometric(x, y);
            SDL_Rect destRect = {
                isoPos.x,
                isoPos.y,
                m_isometricConverter.GetTileWidth(),
                m_isometricConverter.GetTileHeight()
            };
            
            m_reference.AddObject(renderableObject(
				atlasTexture,
				slice->sourceRect,
				destRect,
				isoPos
			));
			
			if( x == m_cursor.x && y == m_cursor.y) // switch for cursor too
			{
				if(!currTile.isWalkable)
				{
					continue;
				}
				
				const sliceInfo* cursorSlice = m_mapIndicatorsParser.GetSliceInfo("Gold_Hover");
				if(cursorSlice) {
					const int HOVER_SCALE_FACTOR = 2;

                    int hoverDestWidth = cursorSlice->sourceRect.w * HOVER_SCALE_FACTOR;
                    int hoverDestHeight = cursorSlice->sourceRect.h * HOVER_SCALE_FACTOR;

                    int hoverOffsetX = (m_isometricConverter.GetTileWidth() - hoverDestWidth) / 2;
                    int hoverDestY_offset = m_isometricConverter.GetTileHeight() - hoverDestHeight;

                    SDL_Rect hoverDestRect = {
                        isoPos.x + (cursorSlice->sourceRect.w / 4),
                        isoPos.y - (cursorSlice->sourceRect.h / 4),
                        hoverDestWidth,
                        hoverDestHeight
                    };
                    
					SDL_Point hoverSortPos = {
                                isoPos.x,
                                isoPos.y + 1 // add a small value to Y to make it sort after the tile
                            };

                    m_reference.AddObject(renderableObject(
                        m_mapIndicatorsParser.GetAtlasTexture(),
                        cursorSlice->sourceRect,
                        hoverDestRect,
                        hoverSortPos
                    ));
				}
			}
        }
    }
    // second pass: render units on top
	m_reference.AddObject(m_player.GetShadowRenderableObject(&m_shadowAtlasParser, &m_isometricConverter));
    m_reference.AddObject(m_player.GetRenderableObject(&m_unitAtlasParser, &m_isometricConverter));
	m_reference.AddObject(m_enemy.GetShadowRenderableObject(&m_shadowAtlasParser, &m_isometricConverter));
	m_reference.AddObject(m_enemy.GetRenderableObject(&m_unitAtlasParser, &m_isometricConverter));

    m_reference.Draw(m_renderer);
    m_ui.Render(m_renderer, m_font);
    
    RenderHUD();
    
    SDL_RenderPresent(m_renderer);
}

void Game::RenderHUD() {
    if (!m_font) return;

    if (m_cursor.x >= 0 && m_cursor.x < m_map.GetWidth() &&
        m_cursor.y >= 0 && m_cursor.y < m_map.GetHeight()) {
        
        const Tile& currTile = m_map.GetTile(m_cursor.x, m_cursor.y);

        std::string hudText = "Tile: (" + std::to_string(m_cursor.x) + ", " + std::to_string(m_cursor.y) + ")";
        hudText += " Walkable: " + std::string(currTile.isWalkable ? "Yes" : "No") + " Unit: " + (currTile.occupyingUnit ? currTile.occupyingUnit->GetName() : "None");

        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, hudText.c_str(), textColor);
        if (!textSurface) {
            SDL_Log("Failed to create text surface: %s", TTF_GetError());
            return;
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
        SDL_FreeSurface(textSurface);
        if (!textTexture) {
            SDL_Log("Failed to create text texture: %s", SDL_GetError());
            return;
        }

        SDL_Rect hudRect = {10, 10, textSurface->w, textSurface->h};
        SDL_RenderCopy(m_renderer, textTexture, nullptr, &hudRect);
        SDL_DestroyTexture(textTexture);
        
        if (selectedUnit) {
            std::string unitText = "Selected Unit: " + selectedUnit->GetName();
            unitText += " Health: " + std::to_string(selectedUnit->GetHealth());
            unitText += " Position: (" + std::to_string(selectedUnit->GetGridX()) + ", " + std::to_string(selectedUnit->GetGridY()) + ")";
    
            SDL_Color textColor = {255, 255, 0, 255}; 
            SDL_Surface* unitSurface = TTF_RenderText_Solid(m_font, unitText.c_str(), textColor);
            if (unitSurface) {
                SDL_Texture* unitTexture = SDL_CreateTextureFromSurface(m_renderer, unitSurface);
                SDL_FreeSurface(unitSurface);
                if (unitTexture) {
                    SDL_Rect unitRect = {10, 30, unitSurface->w, unitSurface->h};
                    SDL_RenderCopy(m_renderer, unitTexture, nullptr, &unitRect);
                    SDL_DestroyTexture(unitTexture);
                }
            }
        }
    
        Unit* currentUnit = m_turnManager.GetCurrentUnit();
        if (currentUnit) {
            std::string turnText = "Current Turn: " + currentUnit->GetName();

            SDL_Color turnTextColor = {0, 255, 0, 255}; 
            SDL_Surface* turnSurface = TTF_RenderText_Solid(m_font, turnText.c_str(), turnTextColor);
            if (turnSurface) {
                SDL_Texture* turnTexture = SDL_CreateTextureFromSurface(m_renderer, turnSurface);
                SDL_FreeSurface(turnSurface);
                if (turnTexture) {
                    int windowWidth, windowHeight;
                    SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

                    SDL_Rect turnRect = {windowWidth - turnSurface->w - 10, 10, turnSurface->w, turnSurface->h};
                    SDL_RenderCopy(m_renderer, turnTexture, nullptr, &turnRect);
                    SDL_DestroyTexture(turnTexture);
                }
            }
        }
    }
}

void Game::Cleanup() {
	if(m_renderer) {
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
	
	if(m_window) {
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
	
	if(m_font) {
		TTF_CloseFont(m_font);
		m_font = nullptr;
	}
	
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	
	SDL_Log("Game cleaned up successfully.");
}