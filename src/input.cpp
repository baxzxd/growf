#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"

void ProcessKey(SDL_KeyboardEvent *key) {
    if( key->keysym.scancode > 32 )
        return;

    if( key->state == SDL_PRESSED )
        keys[key->keysym.scancode] = 1;
    else if( key->state == SDL_RELEASED )
        keys[key->keysym.scancode] = 0;
}