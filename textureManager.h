#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL.h>
#include <string>

class TextureManager {
public:
    static SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);
};

#endif
