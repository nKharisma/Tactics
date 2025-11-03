#pragma once
#include "UIElement.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <functional>

enum class ButtonState {
	NotSelected,
	Hovered,
	Selected
};

class UIButton : public UIElement {
public:
	UIButton(const std::string& text, SDL_Rect rect, std::function<void()> onClick) : m_text(text), m_onClick(onClick), m_state(ButtonState::NotSelected) {
		m_rect = rect;
	}
	void Render(SDL_Renderer* renderer, TTF_Font* font) override {
	SDL_Color color;
	
	switch(m_state) {
		case ButtonState::NotSelected:
			color = {255, 255, 255, 255}; // White
			break;
		case ButtonState::Hovered:
			color = {200, 200, 200, 255}; // Light gray
			break;
		case ButtonState::Selected:
			color = {255, 255, 0, 255}; // Yellow
			break;
	}
	
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, m_text.c_str(), color);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	
	SDL_Rect textRect = {
		m_rect.x + (m_rect.w - textSurface->w) / 2,
		m_rect.y + (m_rect.h - textSurface->h) / 2,
		textSurface->w,
		textSurface->h
	};

	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_DestroyTexture(textTexture);
	SDL_FreeSurface(textSurface);
	}
	
	void HandleMouseEvent(int mouseX, int mouseY, bool isClicked) override {
		if(mouseX >= m_rect.x && mouseX <= m_rect.x + m_rect.w &&
		   mouseY >= m_rect.y && mouseY <= m_rect.y + m_rect.h) {
			if(isClicked) {
				m_onClick();
				m_state = ButtonState::Selected;
			} else if(!isClicked && m_state != ButtonState::Selected) {
				m_state = ButtonState::Hovered;
			}
		} else {
			if (m_state != ButtonState::Selected) {
            m_state = ButtonState::NotSelected;
        }
		}
	}
	
	void SetState(ButtonState state) {
		m_state = state;
	}
	
	bool IsHovered(int mouseX, int mouseY) const {
        return mouseX >= m_rect.x && mouseX <= m_rect.x + m_rect.w &&
               mouseY >= m_rect.y && mouseY <= m_rect.y + m_rect.h;
    }

private:
	std::string m_text;
	std::function<void()> m_onClick;
	ButtonState m_state;

};