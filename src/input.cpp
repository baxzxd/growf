#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"

void ProcessKey(SDL_KeyboardEvent *key) {
    if( key->keysym.scancode > 128 || key->repeat )
        return;
    std::cout<<"key"<<key->keysym.scancode<<std::endl;
    
    if( key->state == SDL_PRESSED ) {
        keys[key->keysym.scancode] = 1;
        keysJustPressed[key->keysym.scancode] = 1;
    }
    else if( key->state == SDL_RELEASED ) {
        keys[key->keysym.scancode] = 0;
    }
}