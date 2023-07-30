
#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "world.h"
#include "../main.h"
#include "../render/render.h"
#include "../color.h"
#include "../FastNoiseLite.h"

#include "../obj/ObjMain.h"
#include "../obj/ObjUtil.h"

FastNoiseLite noiseS;
FastNoiseLite oreNoise;
void Noise_SetParam(FastNoiseLite *n, FastNoiseLite::NoiseType noise, float frequency, int seed)
{
    n->SetNoiseType(noise);
    n->SetFrequency(frequency);
    n->SetSeed(seed);
}
std::vector<float> Noise_GenerateGrid(int res, float scale, float xOff = 0, float yOff = 0)
{
    // Gather noise data
    std::vector<float> noiseData(res * res);
    for (int y = 0; y < res; y++)
    {
        for (int x = 0; x < res; x++)
        {
            noiseData[y * res + x] = noiseS.GetNoise((float)(x + xOff) * scale, (float)(y + yOff) * scale);
        }
    }
    return noiseData;
}
void Noise_Combine(std::vector<float> &a, std::vector<float> &b, int res)
{
    for (int i = 0; i < res * res; i++)
    {
        a[i] = (float)a[i] - (float)b[i];
        if ((float)a[i] < -1)
            a[i] = -1.0f;
        if ((float)a[i] > 1)
            a[i] = 1.0f;
    }
}
void Noise_Print(std::vector<float> &v, int res, float min)
{
    for (int x = 0; x < res; x++)
    {
        for (int y = 0; y < res; y++)
        {
            int i = y + res * x;
            if (v[i] > .4f)
                std::cout << 1;
            else
                std::cout << 0;
        }
        std::cout << std::endl;
    }
}

SDL_Texture *noiseTexture;
SDL_Rect noiseDestRect;
SDL_Rect noiseRect;

// generate noise layers or somethin
int seedA = time(NULL);
int seedB = seedA + rand() % 421424;
// tilegen rules per biome
int chunkSurfaceSize = tileSize * chunkSize;
SDL_Surface *oreTextures[2];
WorldChunk chunks[worldDim][worldDim];
float groundWeights[4] = {.4f, 0};

// tiles dont have height chunks have 4-5 layers?
// void underground ground layer1 layer2

std::string World_GetTileByWeight(float v)
{
    // map of tiles
    for (int j = 0; j < sizeof(groundWeights); j++)
    {
        if (v < groundWeights[j])
            continue;
        switch (j)
        {
        case 0:
            return "grass";
            break;
        case 10:
            return "dirt";

            break;
        case 20:
            return "water";

            break;
        case 1:
            return "stone";
            break;
        }
    }
    return "stone";
}

struct WorldNoiseLayer
{
    CHUNK_LAYERS layer;
    std::string id;
    bool replaceOnly;
    // FastNoiseLite::NoiseType_OpenSimplex2, .01f, seedB
    FastNoiseLite::NoiseType noise;
    float frequency;
    int seed;
    float min;
    float max;
};
std::vector<WorldNoiseLayer> genLayers;
void World_GenerateChunk(int x, int y, std::string tile, CHUNK_LAYERS layer)
{
    // draw noise to texture
    float nX, nY;
    int tX, tY;
    WorldChunk *c = &chunks[x][y];
    WorldTile *t;
    for (tY = 0; tY < chunkSize; tY++)
    {
        for (tX = 0; tX < chunkSize; tX++)
        {
            nX = (float)(tX + x * chunkSize);
            nY = (float)(tY + y * chunkSize);
            for (int i = CHUNK_AIR; i >= 0; i--)
            {
                c->tiles[i][tX][tY].chunk = c;
            }
            t = &c->tiles[CHUNK_GROUND][tX][tY];
            t->layer = CHUNK_GROUND;
            t->data = &tileData["grass"];
            t->pos = {(float)tX * tileSize + (x * tileSize * chunkSize), (float)tY * tileSize + (y * tileSize * chunkSize)};

            for (int i = 0; i < genLayers.size(); i++)
            {
                Noise_SetParam(&noiseS, genLayers[i].noise, genLayers[i].frequency, genLayers[i].seed);
                float v = (noiseS.GetNoise(nX, nY) + 1) / 2;
                if (v < genLayers[i].min)
                    continue;

                t = &c->tiles[layer][tX][tY];
                if (!t->data && genLayers[i].replaceOnly)
                    continue;
                t->layer = layer;
                t->data = &tileData[genLayers[i].id];
                t->pos = {(float)tX * tileSize + (x * tileSize * chunkSize), (float)tY * tileSize + (y * tileSize * chunkSize)};
            }
        }
    }
}

/// start to block layers below after wiring(3)
SDL_Texture *World_ChunkToTexture(int x, int y)
{
    WorldChunk *c = &chunks[x][y];
    WorldTile *t;
    noiseRect.h = tileSize;
    noiseRect.w = tileSize;
    SDL_Surface *noiseSurface = SDL_CreateRGBSurface(0, chunkSurfaceSize, chunkSurfaceSize, 32, 0, 0, 0, 0);
    for (int tX = 0; tX < chunkSize; tX++)
    {
        noiseRect.x = tX * tileSize;
        for (int tY = 0; tY < chunkSize; tY++)
        {
            noiseRect.y = tY * tileSize;
            t = 0;
            for (int i = CHUNK_WIRING; i >= 0; i--)
            {
                if (!c->tiles[i][tX][tY].data)
                    continue;
                t = &c->tiles[i][tX][tY];
                break;
            }
            if (!t) {
                std::cout << "NO TILE DATA AT [" << x << "," << y << "]" << std::endl;
            }
            else {
                // check if there are tiles above or to the left
                if ( tX - 1 > 0 && !c->tiles[t->layer][tX - 1][tY].data ) 
                    noiseRect.x -= 4;
                if ( tY - 1 > 0 && !c->tiles[t->layer][tX][tY - 1].data ) 
                    noiseRect.y -= 4;
                //SDL_FillRect(noiseSurface, &noiseRect, 0x46000000);

                noiseRect.x = tX * tileSize;
                noiseRect.y = tY * tileSize;

                
                if (t->data->textures.size())
                {
                    SDL_BlitSurface(t->data->textures[rand() % t->data->textures.size()], NULL, noiseSurface, &noiseRect);
                }
                else
                {
                    SDL_FillRect(noiseSurface, &noiseRect, Color_RGBToInt(t->data->c.r,t->data->c.g,t->data->c.b));
                }
            }
        }
    }

    // create texture and free surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, noiseSurface);
    SDL_FreeSurface(noiseSurface);

    return texture;
}
SDL_Texture *chunkTextures[worldDim][worldDim];
void World_GenerateLand()
{
    oreTextures[0] = Render_CreateSpeckledSurface(16, 16, Color{128, 128, 128}, Color{190, 190, 0}, 3, 3, 3);
    oreTextures[1] = Render_CreateSpeckledSurface(16, 16, Color{128, 128, 128}, Color{0, 128, 0}, 6, 6, 6);
    for (int x = 0; x < worldDim; x++)
    {
        for (int y = 0; y < worldDim; y++)
        {
            World_GenerateChunk(x, y, "stone", CHUNK_AIR);
            chunkTextures[x][y] = World_ChunkToTexture(x, y);
        }
    }
}
SDL_Rect testRect;
SDL_Texture *testTexture;
double Rand_Double()
{
    return (double)(rand() % 255) / 255.0;
}
SDL_Surface *Noise_TextureWhiteNoise(int res, int sc, float range, Color c)
{
    testRect.h = sc;
    testRect.w = sc;
    SDL_Surface *testSurface = SDL_CreateRGBSurface(0, res, res, 32, 0, 0, 0, 0);
    for (int x = 0; x < res / sc; x++)
    {
        for (int y = 0; y < res / sc; y++)
        {
            Color c2 = c;
            testRect.x = x * sc;
            testRect.y = y * sc;
            double n = Rand_Double() * range + (1 - range) / 2;
            c2.r *= n;
            c2.g *= n;
            c2.b *= n;
            SDL_FillRect(testSurface, &testRect, Color_RGBToInt(c2.r, c2.g, c2.b));
        }
    }
    return testSurface;
}

// make struct to pass change and store chunk location?
WorldChange change;
WorldTile *changeTile;
int changeX, changeY;
std::string changeId;
CHUNK_LAYERS changeLayer;

void AddWorldChange()
{
    int e = -1;
    for (int i = 0; i < change.tileTypes; i++)
    {
        if (change.slots[i].id == changeTile->data->id)
        {
            e = i;
            break;
        }
    }
    if (e == -1)
    {
        e = change.tileTypes;
        change.tileTypes += 1;
        change.slots[e].id = changeTile->data->id;
        change.slots[e].amount = 0;
    }
    change.slots[e].amount += 1;
}
/// if void check nearby tiles to place
int World_SetTile()
{
    changeTile = World_GetTile(changeX, changeY, changeLayer);
    if (!changeTile)
        return 0;
    if (!changeTile->data)
    {
        if (changeId.empty())
            return 0;
    }
    else
    {
        if (changeTile->data->id == changeId)
            return 0;
        else
            AddWorldChange();
    }
    // stone tiles give chunk items that can be comined into tiles
    // x`free standing tiles can be collected mayb
    // stacking with itme inv

    chunks[changeX/chunkSize][changeY/chunkSize].update = true;
    if (changeId.empty())
    {
        changeTile->data = 0;
    }
    else
    {
        changeTile->data = &tileData[changeId];
    }

    return 1;
}
int World_ChangeTile(std::string id, V2Int worldPos, int size, CHUNK_LAYERS layer)
{
    change.tileTypes = 0;
    changeId = id;
    changeLayer = layer;
    int tilesChanged = 0;
    V2Int pos = worldPos / tileSize;
    changeX = pos.x, changeY = pos.y;

    if (size != 0)
    {
        for (int x = -size; x < size + 1; x++)
        {
            for (int y = -size; y < size + 1; y++)
            {
                changeX = pos.x + x;
                changeY = pos.y + y;
                tilesChanged += World_SetTile();
            }
        }
    }
    else
        tilesChanged += World_SetTile();

    if (tilesChanged)
    {
        Particle_PlayEffect(worldPos, 4, Color_RGBToInt(0, 180, 0), {6, 6});
    }
    return tilesChanged;
}
#include "../audio.h"
void World_Init()
{
    Audio *o = createAudio("test.mp3", 0, 64);

    AddTileToData("grass", Color{0, 170, 0}, false);
    AddTileToData("water", Color{0, 0, 160}, true);
    AddTileToData("lava", Color{255, 140, 0}, true);
    AddTileToData("stone", Color{128, 128, 128}, false);
    AddTileToData("goldOre", Color{128, 128, 128}, false);
    AddTileToData("diamondOre", Color{128, 128, 128}, false);
    AddTileToData("dirt", Color{85, 48, 5}, false);

    testRect.h = 1;
    testRect.w = 1;
    SDL_Texture *tex;
    for (int i = 0; i < 4; i++)
    {
        tileData["grass"].textures.push_back(Noise_TextureWhiteNoise(tileSize, 1, .1f, Color{0, 200, 0}));
        tex = SDL_CreateTextureFromSurface(renderer, tileData["grass"].textures[i]);
        tileData["grass"].surfaces.push_back(tex);

        tileData["stone"].textures.push_back(Noise_TextureWhiteNoise(tileSize, 1, .2f, Color{128, 128, 128}));
        tex = SDL_CreateTextureFromSurface(renderer, tileData["stone"].textures[i]);
        tileData["stone"].surfaces.push_back(tex);

        tileData["lava"].surfaces.push_back(Noise_GenerateSimplex(tileSize, 1, .2f, .1f, Color{128, 128, 128}));

        tileData["goldOre"].textures.push_back(Render_CreateSpeckledSurface(tileSize, tileSize, Color{128, 128, 128}, Color{0, 255, 0}, 4, 4, 4));
        tex = SDL_CreateTextureFromSurface(renderer, tileData["goldOre"].textures[i]);
        tileData["goldOre"].surfaces.push_back(tex);

        tileData["diamondOre"].textures.push_back(Render_CreateSpeckledSurface(tileSize, tileSize, Color{128, 128, 128}, Color{16, 32, 255}, 4, 4, 4));
        tex = SDL_CreateTextureFromSurface(renderer, tileData["diamondOre"].textures[i]);
        tileData["diamondOre"].surfaces.push_back(tex);
    }

    noiseS.SetFractalType(FastNoiseLite::FractalType_FBm);
    Noise_SetParam(&noiseS, FastNoiseLite::NoiseType_OpenSimplex2, .05f, seedA);
    Noise_SetParam(&oreNoise, FastNoiseLite::NoiseType_OpenSimplex2, .01f, seedB);

    genLayers.push_back(WorldNoiseLayer{CHUNK_AIR, "stone", false, FastNoiseLite::NoiseType_OpenSimplex2, .02f, seedA, .65f, 0.0f});
    genLayers.push_back(WorldNoiseLayer{CHUNK_AIR, "goldOre", true, FastNoiseLite::NoiseType_OpenSimplex2, .02f, seedB, .9f, 0.0f});
    genLayers.push_back(WorldNoiseLayer{CHUNK_AIR, "diamondOre", true, FastNoiseLite::NoiseType_OpenSimplex2, .01f, seedB, .95f, 0.0f});
    World_GenerateLand();
    // World_SerializeWorld();
    // World_DeserializeWorld();

    Obj_Create("corn", {128, 128}, 1);
    Obj_Create("ak47", {100, 128}, 1);
    cameraScale = 1.0f;
}

void World_RenderChunks()
{
    for (int x = 0; x < worldDim; x++)
    {
        for (int y = 0; y < worldDim; y++)
        {
            if (chunks[x][y].update)
            {
                SDL_DestroyTexture(chunkTextures[x][y]);
                chunkTextures[x][y] = World_ChunkToTexture(x, y);
                chunks[x][y].update = false;
            }
            Render_Copy(chunkTextures[x][y], V2{(float)(x * chunkSurfaceSize), (float)(y * chunkSurfaceSize)}, {(float)chunkSurfaceSize, (float)chunkSurfaceSize});

            // Render_SetDrawColor(Color_RGBToInt(0,255,255),255);
            // SDL_RenderDrawRect(renderer, &noiseDestRect);
        }
    }
}
void World_Render()
{
    World_RenderChunks();
}
WorldTile *World_GetTile(int x, int y, CHUNK_LAYERS layer)
{
    int chunkX = x / chunkSize;
    int tileX = x % chunkSize;
    int chunkY = y / chunkSize;
    int tileY = y % chunkSize;
    if (chunkX < 0 || chunkY < 0 || tileX < 0 || tileY < 0)
    {
        std::cout << "ERROR IN GETTILE" << std::endl;
        return 0;
    }
    return &chunks[chunkX][chunkY].tiles[layer][tileX][tileY];
}
// all this sucks asssss
// get new vel position and check tiles and iterate through each and adjust
WorldTile *World_CheckToMap(Gobj *e)
{
    V2Int p1 = V2Int{e->pos.x, e->pos.y} / tileSize;
    V2Int p2 = V2Int{e->pos.x + e->size.x, e->pos.y + e->size.y} / tileSize;
    WorldTile *t;
    // use get tile function to avoid overflows in chunk tiling
    for (int x = p1.x; x < p2.x + 1; x++) {
        for (int y = p1.y; y < p2.y + 1; y++) {
            t = World_GetTile(x, y, CHUNK_AIR);
            return t;
        }
    }
    return 0;
}