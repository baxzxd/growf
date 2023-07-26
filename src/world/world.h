#ifndef WORLD_H
#define WORLD_H
#include <vector>
#include <SDL2/SDL.h>
#include "../obj/ObjMain.h"
#include "../FastNoiseLite.h"
//gradient for dirt layers
//hold different amounts of liquid and can leach?
void World_GenerateLand();
extern SDL_Rect noiseRect;
extern SDL_Texture *noiseTexture;
extern SDL_Rect noiseDestRect;
const int worldDim = 24;
const int chunkSize = 8;
const int tileSize = 16;

struct TileData {
    std::string id;
    int c;
    int health;
    bool flows = false;
    std::vector<SDL_Surface*> textures;
};

struct WorldChunk;
struct WorldTile {
    int height;//if tile has height its collisionable? total miner minimap for underwater
    int subType;
    V2 pos;
    TileData *data;
    WorldChunk *chunk;
};


struct WorldChunk {
    bool update = false;
    V2Int pos;
    WorldTile tiles[chunkSize][chunkSize]; 
};
extern WorldChunk chunks[worldDim][worldDim];
extern SDL_Texture* chunkTextures[worldDim][worldDim];
extern SDL_Texture *testTexture;
void World_Init();
void World_Render();
SDL_Texture *World_ChunkToTexture(int x, int y);
bool World_CheckToMap(Gobj *obj);
WorldTile *World_GetTile(int x, int y);
extern std::map<std::string, TileData> tileData;
void AddTileToData(std::string id, int c, bool flows);
void World_ChangeTile(std::string id, V2Int pos, int size);
void World_SerializeWorld();
void World_DeserializeWorld();
#endif