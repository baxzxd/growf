#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render/render.h"
#include "../world/world.h"
#include "../obj/ObjMain.h"
#include "../obj/ObjUtil.h"
#include "../obj/ObjPhys.h"
#include "../obj/StandardObjs.h"

float spawnTime = 0;
float spawnTimer = 0;
Gobj *spawns[16];
int spawnss = 0;
void Survival_Tick() {
    for( int i = 0; i < 16; i++ ) {
        if( spawns[i] && spawns[i]->health == -1 ) {
            std::cout<<"dog spawn died"<<std::endl;
            spawns[i]->referencesTo--;
            spawns[i] = 0;
            spawnss--;
        }
    }
    if( spawnss < 16 ) {
        spawnTime -= del;
        if( spawnTime <= 0 ) {
            V2Int chunk = {rand()%worldDim,rand()%worldDim};
            V2Int tile;
            for( int i = 0; i < 5; i++ ) {
                tile = {rand()%chunkSize,rand()%chunkSize};
                if( chunks[chunk.x][chunk.y].tiles[tile.x][tile.y].height )
                    continue;
                
                break;
            }
            int r = -1;
            for( int i = 0; i < 16; i++ ) {
                if( !spawns[i] ) {
                    r = i;
                    break;
                }
            }    
            if( r == -1 )
                return; 
            V2 pos = {(chunk.x * chunkSize + tile.x) * tileSize,(chunk.y * chunkSize + tile.y) * tileSize};
            Gobj *spawn = Obj_Create("dog",pos,RandV2(100),2);
            spawn->team = 1;
            spawn->state = 2;
            spawns[r] = spawn;
            spawnss++;
            spawn->referencesTo++;
            spawnTime = rand()%2+1;
        }
    }
}