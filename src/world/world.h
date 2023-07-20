#ifndef WORLD_H
#define WORLD_H
#include <SDL2/SDL.h>
//gradient for dirt layers
//hold different amounts of liquid and can leach?
void World_GenerateLand();

extern SDL_Rect noiseRect;
extern SDL_Texture *noiseTexture;
extern SDL_Rect noiseDestRect;
const int worldDim = 24;

extern SDL_Texture* chunkTextures[worldDim][worldDim];
extern int chunkSize;
extern int tileSize;
#endif