#ifndef WORLD_H
#define WORLD_H
#include <SDL2/SDL.h>
#include "../obj/ObjMain.h"
//gradient for dirt layers
//hold different amounts of liquid and can leach?
void World_GenerateLand();

extern SDL_Rect noiseRect;
extern SDL_Texture *noiseTexture;
extern SDL_Rect noiseDestRect;
const int worldDim = 24;
const int chunkSize = 8;


struct WorldTile {
    int id;
    int height;//if tile has height its collisionable? total miner minimap for underwater
    bool flows = false;
    int subType;
    V2 pos;
};
extern WorldTile tiles[worldDim*chunkSize][worldDim*chunkSize];
bool World_CheckToMap(Gobj *obj);
extern SDL_Texture* chunkTextures[worldDim][worldDim];
extern int tileSize;
#endif