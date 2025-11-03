#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>
#include <vector>
#include "UIElement.h"
#include "UIButton.h"

class UI {
public: 
	void AddElement(std::shared_ptr<UIElement> element)
	{
		m_elements.push_back(element);
	}
	
	void DeselectAllButtons() {
			for (auto& element : m_elements) {
		// Check if the element is a UIButton
		auto button = std::dynamic_pointer_cast<UIButton>(element);
		if (button) {
			button->SetState(ButtonState::NotSelected);
		}
	}
	}
	
	void Render(SDL_Renderer* renderer, TTF_Font* font)
	{
		for(auto& element : m_elements) {
			element->Render(renderer, font);
		}
	}
	
	void HandleMouseEvent(int mouseX, int mouseY, bool isClicked)
	{
		for(auto& element : m_elements) {
			element->HandleMouseEvent(mouseX, mouseY, isClicked);
			
			if(isClicked) {
				auto button = std::dynamic_pointer_cast<UIButton>(element);
				if(button && button->IsHovered(mouseX, mouseY)) {
					DeselectAllButtons();
					button->SetState(ButtonState::Selected);
					break;
				}
			}
		}
	}
	
private:
	std::vector<std::shared_ptr<UIElement>> m_elements;
};

#endif