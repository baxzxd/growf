#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"
#include "color.h"
#include "render.h"

SDL_Rect drawTexRect = {0,0,2,2};
SDL_Texture *genTexture;
SDL_Surface *Render_CreateSpeckledSurface(int w, int h, int baseColor, int sColor, int sAmount, int sW, int sH) {
    SDL_Surface *s = SDL_CreateRGBSurface(0,16,16,32,0,0,0,0);  
    
    SDL_FillRect(s,NULL, baseColor);
    
    drawTexRect.w = sH;
    drawTexRect.h = sW;
    for( int i = 0; i < sAmount; i++ ) {
        drawTexRect.x = rand()&(w-sW);
        drawTexRect.y = rand()&(h-sH);
        SDL_FillRect(s, &drawTexRect, sColor);
    }

    return s;
}
SDL_Texture *Render_CreateSpeckledTexture(int w, int h, int baseColor, int sColor, int sAmount, int sW, int sH) {
    SDL_Surface *s = Render_CreateSpeckledSurface(w,h,baseColor,sColor,sAmount,sW,sH);
    // create texture and free surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, s);
    SDL_FreeSurface(s);
    return texture;
}