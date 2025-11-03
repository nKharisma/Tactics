#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>

#include "renderable_object.h"

class Draw_Game {
	public:
		Draw_Game();
		~Draw_Game();
		
		void AddObject(const renderableObject& obj);
		void Draw(SDL_Renderer* renderer);
		void ClearObjects();
		void revealSpeed(double speed);
		void Update(double deltaTime);
		
	private:
		std::vector<renderableObject> m_objects;
		double m_speed;
		double m_drawTime;
};

#endif