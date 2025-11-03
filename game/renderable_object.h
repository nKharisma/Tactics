#ifndef RENDERABLE_OBJECT_H
#define RENDERABLE_OBJECT_H

#include <SDL2/SDL.h>

struct renderableObject {
    SDL_Texture* texture;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    SDL_Point isoPosition;
    
    renderableObject(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_Point isoPos)
        : texture(tex), srcRect(src), destRect(dest), isoPosition(isoPos) {}
};

#endif