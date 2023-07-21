
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

#include "../obj/ObjMain.h"
#include "../obj/ObjUtil.h"

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


// generate noise layers or somethin    
int seedA = time(NULL); 
int seedB = seedA + rand() % 128;
int tileSize = 16;

int chunkSurfaceSize = tileSize * chunkSize; 
SDL_Surface *oreTextures[2];
WorldTile tiles[worldDim*chunkSize][worldDim*chunkSize];

SDL_Texture* World_GenerateChunkTexture(int x, int y) {
    noiseDestRect.w = chunkSurfaceSize;
    noiseDestRect.h = chunkSurfaceSize;
    noiseRect.h = tileSize;
    noiseRect.w = tileSize;
    std::vector<float> base = Noise_GenerateGrid(chunkSize, .05f, 1.0f,seedA, x * chunkSize, y * chunkSize);
    // draw noise to texture 
    SDL_Surface *noiseSurface = SDL_CreateRGBSurface(0,chunkSurfaceSize,chunkSurfaceSize,32,0,0,0,0);  
    for( int i = 0; i < chunkSize*chunkSize; i++ ) {
        int tileX = (i % chunkSize);
        int tileY = (i / chunkSize);
        noiseRect.x = tileX * tileSize;
        noiseRect.y = tileY * tileSize;
        float v = ((float)base[i] + 1) / 2;
        float tileWeights[] = {1, .5f,.7f,.65f,.3f};
        int tile = 0;
        for( int j = 1; j < sizeof(tileWeights); j++ ) {
            if( v > tileWeights[j] ) {
                tile = j;
                break;
            }
        }

        int tX = tileX+ x*chunkSize;
        int tY = tileY + y*chunkSize;
        WorldTile *t = &tiles[tX][tY];
        t->pos = {(float)tX*tileSize,(float)tY*tileSize};
        int c;
        int colorSteps = 3;
        int d = tileWeights[tile-1] - tileWeights[tile];
        if( tile == 1 ) 
            //lerp grass
            c = Color_LerpColor(Color_RGBToInt(0,180,0),Color_RGBToInt(0,86,0), (float)tile/8.0f);
            
        else {
            c = 0;
        }
        
        if( c != 0 ) {
            SDL_FillRect(noiseSurface, &noiseRect, c);
            t->id = 1;
        }
        else {
            if( rand() % 10 == 0 ) {
                SDL_BlitSurface(oreTextures[0], NULL, noiseSurface, &noiseRect);
            }
            else {
                c = Color_RGBToInt(128,128,128);
                SDL_FillRect(noiseSurface, &noiseRect, c);
                t->height = 2;
            }
            t->id = 2;
            t->height = 3;
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
    for( int x = 0; x < worldDim; x++ ) {
        for( int y = 0; y < worldDim; y++ ) {
            chunkTextures[x][y] = World_GenerateChunkTexture(x,y);
        }
    }


    int validTreeTile = 0;
    Gobj *tree;
    for( int x = 0; x < chunkSize * worldDim; x++ ) {
        for( int y = 0; y < chunkSize * worldDim; y++ ) {
            if( !tiles[x][y].height ) {
                validTreeTile++;
                if( validTreeTile > 500 && rand()%4==0 ) {
                    std::cout<<"CREATEEE TEEE"<<std::endl;
                    tree = Obj_Create("tree", V2{(float)(x*tileSize),(float)(y*tileSize)}, .2f+(float)(rand()%10)/10.0f);
                    tree->data->funcs->funcUse(tree);
                    validTreeTile = 0;
                }
            }
        
        }
    }
    //std::vector<float> base = 
    //std::vector<float> layerA = Noise_GenerateGrid(chunkSize, .014f, seedB);
    //Noise_Combine(base, layerA, chunkSize);

    //water checks surrounding tiles subType and adds to it if its the least
}



//all this sucks asssss
//get new vel position and check tiles and iterate through each and adjust
bool World_CheckToMap(Gobj *e) {
    V2Int p1 = V2Int{e->pos.x, e->pos.y} / tileSize;
    V2Int p2 = V2Int{e->pos.x + e->size.x, e->pos.y + e->size.y} / tileSize;
    for( int x = p1.x; x < p2.x; x++ ) {
        for( int y = p1.y; y < p2.y; y++ ) {

            if( e->id == "player" )
            if( tiles[x][y].height )
                return true;
        }
    }
    return false;
}
void World_CheckToMapOl(Gobj *e) {
    int i;
    V2Int p1,p2;
    V2 p1V,p2V;
    float x = e->pos.x;
    float y = e->pos.y;
    V2 eVel = e->vel * del;
    /* Test the horizontal movement first */
    i = e->size.y > tileSize ? tileSize : e->size.y;
    for (;;) {
        p1V = (e->pos + V2{ eVel.x, 0 }) / tileSize;
        p2V = (e->pos + V2{ eVel.x + e->size.x - 1, (float)i-1 }) / tileSize;
        p1 = {(int)p1.x,(int)p1.y};
        p2 = {(int)p2.x,(int)p2.y};
        if (p1.x >= 0 && p2.x < worldDim && p1.y >= 0 && p2.y < worldDim) {
            if (eVel.x > 0) {
                /* Trying to move right */
                std::cout<<tiles[p2.x][p1.y].id<<std::endl;
                if (tiles[p2.x][p1.y].height || tiles[p2.x][p2.y].height) {
                    x = p2.x * tileSize - e->size.x + 1;
                    //e->vel.x = 0;
                }
            }
            else if (eVel.x < 0) {
                /* Trying to move left */
                if (tiles[p1.x][p1.y].height || tiles[p1.x][p2.y].height) {
                    x = (p1.x + 1) * tileSize;
                    //e->vel.x = 0;
                }
            }
        }

        if (i == e->size.y)
            break;

        i += tileSize;

        if (i > e->size.y)
            i = e->size.y;
    }

    /* Now test the vertical movement */

    i = e->size.x > tileSize ? tileSize : e->size.x;

    for (;;) {
        //new 
        p1V = (e->pos + V2{0, eVel.y}) / tileSize;
        p2V = (e->pos + V2{(float)i, eVel.y + e->size.y}) / tileSize;
        p1 = {(int)p1V.x,(int)p1V.y};
        p2 = {(int)p2V.x,(int)p2V.y};

        if (p1.x >= 0 && p2.x < worldDim && p1.y >= 0 && p2.y < worldDim) {
            if (eVel.y > 0) {
                /* Trying to move down */
                if (tiles[p2.y][p1.x].height || tiles[p2.y][p2.x].height ) {
                    e->size.y = p2.y * tileSize - e->size.y;
                    //e->vel.y = 0;
                }
            }
            else if (eVel.y < 0) {
                /* Trying to move up */
                if ( tiles[p1.x][p1.y].height || tiles[p2.x][p1.y].height ) {
                    y = (p1.y + 1) * tileSize;
                    //e->vel.y = 0;
                }
            }
        }

        if (i == e->size.x)
            break;

        i += tileSize;

        if (i > e->size.x)
            i = e->size.x;
    }

    e->pos = {x,y};
    /* Now apply the movement */
}