#ifndef ISOMETRIC_CONVERTER_H
#define ISOMETRIC_CONVERTER_H

#include <SDL2/SDL.h>

class IsometricConverter {
public:
	IsometricConverter(int tileWidth, int tileHeight, int offsetX = 0, int offsetY = 0);
	
	SDL_Point WorldToIsometric(int worldX, int worldY) const;
	SDL_Point IsometricToWorld(int isoX, int isoY) const;
	
	
	int GetTileWidth() const { return m_tileWidth; }
	int GetTileHeight() const { return m_tileHeight; }
	
private:
	int m_tileWidth;
	int m_tileHeight;
	int m_offsetX;
	int m_offsetY;
};

#endif // ISOMETRIC_CONVERTER_H