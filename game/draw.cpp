#include "draw.h"
#include <SDL2/SDL.h>

Draw_Game::Draw_Game()
	: m_speed(112.0), m_drawTime(0.0)
{
}

Draw_Game::~Draw_Game()
{
	ClearObjects();
}

void Draw_Game::AddObject(const renderableObject& obj)
{
	m_objects.push_back(obj);
}

void Draw_Game::Draw(SDL_Renderer* renderer)
{
	std:sort (m_objects.begin(), m_objects.end(), [](const renderableObject& a, const renderableObject& b) {
		if(a.isoPosition.y != b.isoPosition.y) {
			return a.isoPosition.y < b.isoPosition.y; // Sort by Y position first
		}
		
		return a.isoPosition.x < b.isoPosition.x; // Then by X position
	});
	
	for(size_t i = 0; i < m_objects.size(); ++i) {
		if(m_drawTime >= static_cast<double>(i))
		{
			const renderableObject& obj = m_objects[i];
			
			SDL_RenderCopy(
				renderer,
				obj.texture,
				&obj.srcRect,
				&obj.destRect
			);
		} else {
			break;
		}
	}
}

void Draw_Game::ClearObjects()
{
	m_objects.clear();
}

void Draw_Game::revealSpeed(double speed)
{
	m_speed = speed;
}

void Draw_Game::Update(double deltaTime)
{
	m_drawTime += m_speed * deltaTime; 
	
	if(!m_objects.empty()) {
		m_drawTime = std::min(m_drawTime, static_cast<double>(m_objects.size()));
	} else {
		m_drawTime = 0.0; // Reset draw time if no objects
	}
}