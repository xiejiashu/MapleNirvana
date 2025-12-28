#pragma once
#include "Property.hpp"
#include <SDL3/SDL.h>

struct Texture
{
    static SDL_Texture *load(wz::Property<wz::WzCanvas> *node);
    static SDL_Texture *createBlankTexture(SDL_PixelFormat format, int width, int height);
};