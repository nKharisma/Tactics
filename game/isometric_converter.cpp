#include "isometric_converter.h"

IsometricConverter::IsometricConverter(int tileWidth, int tileHeight, int offsetX, int offsetY)
    : m_tileWidth(tileWidth), m_tileHeight(tileHeight), m_offsetX(offsetX), m_offsetY(offsetY) 
{
}

SDL_Point IsometricConverter::WorldToIsometric(int worldX, int worldY) const {
    SDL_Point isoPosition;

    // diamond isometric projection formula
    isoPosition.x = (worldX - worldY) * (m_tileWidth / 2) + m_offsetX;
    isoPosition.y = (worldX + worldY) * ((m_tileHeight / 2) / 2) + m_offsetY;

    return isoPosition;
}

SDL_Point IsometricConverter::IsometricToWorld(int isoX, int isoY) const {
    SDL_Point worldPosition;

    // inverse of the diamond isometric projection formula
    worldPosition.x = ((isoX - m_offsetX) / (m_tileWidth / 2) + (isoY - m_offsetY) / ((m_tileHeight / 2) / 2)) / 2;
    worldPosition.y = ((isoY - m_offsetY) / ((m_tileHeight / 2) / 2) - (isoX - m_offsetX) / (m_tileWidth / 2)) / 2;

    return worldPosition;
}