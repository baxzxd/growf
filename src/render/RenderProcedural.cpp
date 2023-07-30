#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "render.h"
#include "../world/world.h"
#include "../FastNoiseLite.h"

SDL_Rect drawTexRect = {0,0,2,2};
SDL_Texture *genTexture;
SDL_Surface *Render_CreateSpeckledSurface(int w, int h, Color baseColor, Color sColor, int sAmount, int sW, int sH) {
    SDL_Surface *s = SDL_CreateRGBSurface(0,16,16,32,0,0,0,0);  
    
    SDL_FillRect(s,NULL, Color_RGBToInt(baseColor.r, baseColor.g, baseColor.b));
    
    drawTexRect.w = sH;
    drawTexRect.h = sW;
    for( int i = 0; i < sAmount; i++ ) {
        drawTexRect.x = rand()&(w-sW);
        drawTexRect.y = rand()&(h-sH);
        SDL_FillRect(s, &drawTexRect, Color_RGBToInt(sColor.r, sColor.g, sColor.b));
    }

    return s;
}
SDL_Texture *Render_CreateSpeckledTexture(int w, int h, Color baseColor, Color sColor, int sAmount, int sW, int sH) {
    SDL_Surface *s = Render_CreateSpeckledSurface(w,h,baseColor,sColor,sAmount,sW,sH);
    // create texture and free surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, s);
    SDL_FreeSurface(s);
    return texture;
}
FastNoiseLite renderNoise;
float GetRadialValue(V2 p, int res) {
    return ( V2{(float)(res/2),(float)(res/2)} - p).Len() / (float)res;
}
SDL_Texture *Noise_GenerateSimplex(int res, int sc, float freq, float min, Color c) {
    drawTexRect.h = sc;
    drawTexRect.w = sc;

    renderNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    renderNoise.SetFrequency(freq);
    renderNoise.SetSeed(rand()%500000);
    renderNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    SDL_Surface *testSurface = SDL_CreateRGBSurface(0,res,res,32,0,0,0,0); 
    for( int x = 0; x < res / sc; x++ ) {
        for( int y = 0; y < res / sc; y++ ) {
            float noiseVal = (renderNoise.GetNoise((float)x,(float)y) + 1) / 2;
            //float v = (noiseVal - GetRadialValue(V2{(float)x,(float)y}, res)) / 2;
            //if( v < min )
            //    v = 0;
            drawTexRect.x = (int)(x * sc);
            drawTexRect.y = (int)(y * sc);
            SDL_FillRect(testSurface, &drawTexRect, Color_RGBToInt(c.r*noiseVal,c.r*noiseVal,c.r*noiseVal));
        }
    }

    // render certain amount of frames for lava and lerp between last and first few between cycles? average out?


    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, testSurface);
    SDL_FreeSurface(testSurface);
    return tex;
}