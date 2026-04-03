#include "unit.h"
#include "SDL2/SDL.h"
#include "../json/atlas_parser.h"
#include "map.h"
#include "aStar.h"
#include "turnmanager.h"
#include "isometric_converter.h"
#include "renderable_object.h"

Unit::Unit(const std::string& name, int health, int attackPower, int defense, int attackRange, int gridX, int gridY)
    : m_name(name), m_health(health), m_attackPower(attackPower), m_defense(defense),
      m_attackRange(attackRange), m_gridX(gridX), m_gridY(gridY) 
{
}

void Unit::Update(double deltaTime, Map& map, aStar& pathfinder) {
    // update animation state 
    Animation& currAnim = m_animations[m_animationState];
    currAnim.elapsedTime += deltaTime;

    if (currAnim.elapsedTime >= currAnim.frameDuration) {
        currAnim.elapsedTime = 0.0;

        if (m_animationState == AnimationState::Attacking) {
            if (currAnim.currentFrame < currAnim.frames.size() - 1) {
                currAnim.currentFrame++;
            } else {
                m_attackAnimationTimer = 0.0;
                SetAnimationState(AnimationState::Idle);
            }
        } else {
            currAnim.currentFrame = (currAnim.currentFrame + 1) % currAnim.frames.size();
        }
    }
    static double movementTimer = 0.0;
    movementTimer += deltaTime;

    if (!m_path.empty() && movementTimer >= 0.2) {
        movementTimer = 0.0;

        SDL_Point nextPoint = m_path.front();

        if (nextPoint.x < 0 || nextPoint.x >= map.GetWidth() ||
            nextPoint.y < 0 || nextPoint.y >= map.GetHeight()) {
            SDL_Log("Invalid path step (%d,%d) — clearing path", nextPoint.x, nextPoint.y);
            m_path.clear();
            SetAnimationState(AnimationState::Idle);
            return;
        }

        UpdateTile(map, nextPoint);
        m_path.erase(m_path.begin());
        
    } else if (m_path.empty() && m_animationState != AnimationState::Dead && m_animationState != AnimationState::Attacking) {
        SetAnimationState(AnimationState::Idle);
    }
}

void Unit::MoveTo(int x, int y, Map& map, aStar& pathfinder) {
    if (!m_path.empty() && m_path.back().x == x && m_path.back().y == y) {
        SDL_Log("Already moving to the target position.");
        return;
    }

    if (!map.GetTile(x, y).isWalkable) {
        SDL_Log("Target tile (%d,%d) is not walkable", x, y);
        return;
    }

    m_path = pathfinder.FindPath(m_gridX, m_gridY, x, y, map);

    SDL_Log("New path length = %d", (int)m_path.size());
    for (auto& p : m_path) {
        SDL_Log("  -> (%d,%d)", p.x, p.y);
    }
}

void Unit::SetPosition(int x, int y, Map& map) {
    m_gridX = x;
    m_gridY = y;

    SDL_Point newPoint = { x, y };
    UpdateTile(map, newPoint);
}

void Unit::UpdateTile(Map& map, SDL_Point nextPoint) {
    Tile& currentTile = map.GetTile(m_gridX, m_gridY);
    currentTile.occupyingUnit = nullptr;
    
    m_gridX = nextPoint.x;
	m_gridY = nextPoint.y;
	
	Tile& newTile = map.GetTile(m_gridX, m_gridY);
	newTile.occupyingUnit = this;
}

void Unit::Attack(Unit& target, TurnManager* turnManager) {
    int distance = abs(target.GetGridX() - m_gridX) + abs(target.GetGridY() - m_gridY);
    if (distance > m_attackRange) {
        SDL_Log("Target out of range. Distance: %d, Attack Range: %d", distance, m_attackRange);
        return;
    }

    if (target.GetHealth() > 0) {
        int damage = m_attackPower - target.GetDefense();
        if (damage > 0) {
            m_attackAnimationTimer = 0.0;
            SetAnimationState(AnimationState::Attacking);
            target.TakeDamage(damage, turnManager);
            SDL_Log("%s attacked %s for %d damage!", m_name.c_str(), target.GetName().c_str(), damage);
        } else {
            SDL_Log("%s's attack was too weak to damage %s.", m_name.c_str(), target.GetName().c_str());
        }
    }
}

void Unit::TakeDamage(int damage, TurnManager* turnManager) {
    m_health -= damage;
    if (m_health <= 0) {
        m_health = 0;
        HandleDeath();
        if (turnManager) {
            turnManager->RemoveUnit(this);
        }
    }
}

void Unit::HandleDeath()
{
    SDL_Log("%s has died.", m_name.c_str());
    
    SetAnimationState(AnimationState::Dead);
}

void Unit::SetAnimationState(AnimationState state) {
    if (m_animationState != state) {
        m_animationState = state;
        m_animations[m_animationState].currentFrame = 0;
        m_animations[m_animationState].elapsedTime = 0.0;
    }
}

void Unit::LoadAnimations(const AtlasParser* atlasParser) {
    Animation idleAnimation;
    idleAnimation.frames = {
        atlasParser->GetSliceInfo(m_name + "_Idle_01")->sourceRect,
        atlasParser->GetSliceInfo(m_name + "_Idle_02")->sourceRect
    };
    idleAnimation.frameDuration = 0.5;
    m_animations[AnimationState::Idle] = idleAnimation;

    Animation attackingAnimation;
    attackingAnimation.frames = {
        atlasParser->GetSliceInfo(m_name + "_Attack_01")->sourceRect,
        atlasParser->GetSliceInfo(m_name + "_Attack_02")->sourceRect
    };
    attackingAnimation.frameDuration = 0.2;
    m_animations[AnimationState::Attacking] = attackingAnimation;

    Animation deathAnimation;
    deathAnimation.frames = {
        atlasParser->GetSliceInfo("Tombstone")->sourceRect
    };
    deathAnimation.frameDuration = 1.0;
    m_animations[AnimationState::Dead] = deathAnimation;
}

renderableObject Unit::GetRenderableObject(const AtlasParser* atlasParser, const IsometricConverter* isoConverter) const {
    const Animation& currentAnimation = m_animations.at(m_animationState);
    const SDL_Rect& sliceRect = currentAnimation.frames[currentAnimation.currentFrame];
    
    SDL_Point isoPos = GetScreenPosition(isoConverter);
    const float scale = 2.5f;
    int destYOffset = sliceRect.h - (isoConverter->GetTileHeight());
    int destXOffset = sliceRect.w / 2;

    SDL_Rect destRect = {
        isoPos.x + destXOffset,
        isoPos.y + destYOffset,
        static_cast<int>(sliceRect.w * scale),
        static_cast<int>(sliceRect.h * scale),
    };
    
    SDL_Point unitSort = {
        isoPos.x,
        isoPos.y + 3
    };
    
    return renderableObject(
        atlasParser->GetAtlasTexture(), 
        sliceRect,
        destRect,
        unitSort
    );
}

renderableObject Unit::GetShadowRenderableObject(const AtlasParser* shadowAtlasParser, const IsometricConverter* isoConverter) const {
    const sliceInfo* shadowSlice = shadowAtlasParser->GetSliceInfo("shadow");
    if (!shadowSlice) {
        SDL_Log("No slice info found for shadow.");
        return renderableObject(nullptr, SDL_Rect(), SDL_Rect(), SDL_Point());
    }

    SDL_Point isoPos = GetScreenPosition(isoConverter);

    SDL_Rect shadowDestRect = {
        isoPos.x + (shadowSlice->sourceRect.w / 4),
        isoPos.y + (shadowSlice->sourceRect.h - (isoConverter->GetTileHeight() / 2)),
        static_cast<int>(shadowSlice->sourceRect.w * 0.8f),
        static_cast<int>(shadowSlice->sourceRect.h * 0.5f)
    };

    SDL_Point shadowSort = {
        isoPos.x,
        isoPos.y + 2
    };

    return renderableObject(
        shadowAtlasParser->GetAtlasTexture(),
        shadowSlice->sourceRect,
        shadowDestRect,
        shadowSort
    );
}

SDL_Point Unit::GetScreenPosition(const IsometricConverter* isoConverter) const {
    return isoConverter->WorldToIsometric(m_gridX, m_gridY);
}
