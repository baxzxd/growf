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
const int CHUNK_LAYER_COUNT = 3;
typedef enum CHUNK_LAYERS {
    CHUNK_VOID,
    CHUNK_GROUND,
    CHUNK_AIR,
    CHUNK_WIRING, // wiring can be rendered above or below? hoppers
    CHUNKS_ALL
};

struct InventorySlot {
    int tiles;
    std::string id;
    int amount = 0;
};
struct WorldChange {
    int tileTypes;
    InventorySlot slots[8];
};
extern WorldChange change;
struct TileData {
    std::string id;
    int c;
    int health;
    CHUNK_LAYERS layerRestriction = CHUNKS_ALL;
    bool flows = false;

    // generated
    std::vector<SDL_Surface*> textures;
    std::vector<SDL_Texture*> surfaces;
};

struct WorldChunk;
struct WorldTile {
    char height;//if tile has height its collisionable? total miner minimap for underwater
    char subType;
    V2 pos;
    char layer;
    char smell[8];
    TileData *data;
    WorldChunk *chunk;
};

struct WorldChunk {
    bool update = false;
    V2Int pos;
    WorldTile tiles[CHUNK_LAYER_COUNT][chunkSize][chunkSize]; 
};
extern WorldChunk chunks[worldDim][worldDim];
extern SDL_Texture* chunkTextures[worldDim][worldDim];
extern SDL_Texture *testTexture;
void World_Init();
void World_Render();
SDL_Texture *World_ChunkToTexture(int x, int y);
WorldTile *World_CheckToMap(Gobj *obj);

///go through layers from top
//WorldTile *World_GetTile(int x, int y);
WorldTile *World_GetTile(int x, int y, CHUNK_LAYERS layer);
extern std::map<std::string, TileData> tileData;
void AddTileToData(std::string id, int c, bool flows);
int World_ChangeTile(std::string id, V2Int pos, int size);
void World_SerializeWorld();
void World_DeserializeWorld();
#endif