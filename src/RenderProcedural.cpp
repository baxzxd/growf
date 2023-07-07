#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"
#include "color.h"
#include "render.h"

SDL_Rect drawTexRect = {0,0,8,8};
SDL_Texture *genTexture;
void Render_CreateSpeckledTexture(int w, int h, int baseColor, int sColor, int sAmount, int sMin, int sMax = -1) {
    genTexture = SDL_CreateTexture(renderer,  SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, 32,32);

    SDL_SetRenderTarget(renderer,genTexture);
    Render_SetDrawColor(Color_RGBToInt(128,128,128),255);

    SDL_RenderClear(renderer);
    
    Render_SetDrawColor(Color_RGBToInt(0,0,0),255);
    for( int i = 0; i < 4; i++ ) {
        drawTexRect.x = rand()&30;
        drawTexRect.y = rand()&30;
        SDL_RenderFillRect(renderer,&drawTexRect);

    }
    SDL_SetRenderTarget(renderer,NULL);
}