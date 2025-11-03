#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class UIElement {
public:
	virtual ~UIElement() = default;

	virtual void Render(SDL_Renderer* renderer, TTF_Font* font) = 0;
	virtual void HandleMouseEvent(int mouseX, int mouseY, bool isClicked) {
	
	}

	void SetPosition(int x, int y) { m_rect.x = x; m_rect.y = y; }
	void SetSize(int width, int height) { m_rect.w = width; m_rect.h = height; }

protected:
	SDL_Rect m_rect;
	std::string m_text;
};

#endif