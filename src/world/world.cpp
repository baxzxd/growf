
#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "world.h"
#include "../main.h"
#include "../render.h"
#include "../color.h"
#include "../FastNoiseLite.h"

std::vector<float> Noise_GenerateGrid(int res, float frequency, float scale, int seed, int xOff = 0, int yOff = 0) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(frequency);
    noise.SetSeed(seed);
    // Gather noise data
    std::vector<float> noiseData(res * res);
    int index = 0;

    for (int y = 0; y < res; y++) {
        for (int x = 0; x < res; x++) {
            noiseData[index++] = noise.GetNoise((float)(x + xOff)*scale, (float)(y+ yOff)*scale);
        }
    }
    return noiseData;
}
void Noise_Combine(std::vector<float>& a,std::vector<float>& b, int res) {
    for( int i = 0; i < res*res; i++ ) {
        a[i] = (float)a[i] - (float)b[i];
        if( (float)a[i] < -1 )
            a[i] = -1.0f;
        if( (float)a[i] > 1 )
            a[i] = 1.0f;
    }
}
void Noise_Print(std::vector<float>& v, int res, float min) {
    for( int y = 0; y < res; y++ ) {
        for( int x = 0; x < res; x++ ) {
            int i = y * res + x;
            if( v[i] > .4f )
                std::cout<<1;
            else
                std::cout<<0;
        }
        std::cout<<std::endl;
    }
}
    SDL_Texture *noiseTexture;
    SDL_Rect noiseDestRect;
    SDL_Rect noiseRect;
struct WorldTile {
    int id;
    int height;
    bool flows = false;
    int subType;
};

// generate noise layers or somethin    
int seedA = time(NULL); 
int seedB = seedA + rand() % 128;
int tileSize = 16;
int chunkSize = 8;
int chunkSurfaceSize = tileSize * chunkSize; 
SDL_Surface *oreTextures[2];

SDL_Texture* World_GenerateChunkTexture(int x, int y) {
    noiseDestRect.w = chunkSurfaceSize;
    noiseDestRect.h = chunkSurfaceSize;
    noiseRect.h = tileSize;
    noiseRect.w = tileSize;
    std::vector<float> base = Noise_GenerateGrid(chunkSize, .05f, 1.0f,seedA, x * chunkSize, y * chunkSize);
    // draw noise to texture 
    SDL_Surface *noiseSurface = SDL_CreateRGBSurface(0,chunkSurfaceSize,chunkSurfaceSize,32,0,0,0,0);  
    for( int i = 0; i < chunkSize*chunkSize; i++ ) {
        noiseRect.x = (i % chunkSize) * tileSize;
        noiseRect.y = (i / chunkSize) * tileSize;
        float v = ((float)base[i] + 1) / 2;
        float tileWeights[] = {1, .6f,.7f,.65f,.3f};
        int tile = 0;
        for( int j = 1; j < sizeof(tileWeights); j++ ) {
            if( v > tileWeights[j] ) {
                tile = j;
                break;
            }
        }

        int c;
        int colorSteps = 3;
        int d = tileWeights[tile-1] - tileWeights[tile];
        if( tile == 1 ) 
            //lerp grass
            c = Color_LerpColor(Color_RGBToInt(0,255,0),Color_RGBToInt(0,86,0), (float)tile/8.0f);
        else {
            c = 0;
        }
        
        if( c != 0 ) {
            SDL_FillRect(noiseSurface, &noiseRect, c);
    
        }
        else {
            if( rand() % 10 == 0 ) {
                SDL_BlitSurface(oreTextures[0], NULL, noiseSurface, &noiseRect);
            }
            else {
                c = Color_RGBToInt(128,128,128);
                SDL_FillRect(noiseSurface, &noiseRect, c);
            }
        }
    }
     
    // create texture and free surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer,noiseSurface);
    SDL_FreeSurface(noiseSurface);
    return texture;
}
SDL_Texture *chunkTextures[worldDim][worldDim];
void World_GenerateLand() {
    oreTextures[0] = Render_CreateSpeckledSurface(16,16,Color_RGBToInt(128,128,128),Color_RGBToInt(190,190,0), 3, 3, 3);
    oreTextures[1] = Render_CreateSpeckledSurface(16,16,Color_RGBToInt(128,128,128),Color_RGBToInt(0,128,0), 6, 6, 6);
    for( int y = 0; y < worldDim; y++ ) {
        for( int x = 0; x < worldDim; x++ ) {
            chunkTextures[x][y] = World_GenerateChunkTexture(x,y);
        }
    }
    //std::vector<float> base = 
    //std::vector<float> layerA = Noise_GenerateGrid(chunkSize, .014f, seedB);
    //Noise_Combine(base, layerA, chunkSize);

    //water checks surrounding tiles subType and adds to it if its the least
}