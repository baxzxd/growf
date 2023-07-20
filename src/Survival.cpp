#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"
#include "color.h"
#include "render.h"

#include "obj/ObjMain.h"
#include "obj/ObjUtil.h"
#include "obj/ObjPhys.h"
#include "obj/StandardObjs.h"

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
    if( spawnss < 5 ) {
        spawnTime -= del;
        if( spawnTime <= 0 ) {
            V2 pos = {(float)(rand()%SCREEN_WIDTH), 0.0f};
            int r = -1;
            for( int i = 0; i < 16; i++ ) {
                if( !spawns[i] ) {
                    r = i;
                    break;
                }
            }    
            if( r == -1 )
                return; 
                
            Gobj *spawn = Obj_Create("dog",pos,RandV2(100),2);
            spawn->team = 1;
            spawns[r] = spawn;
            spawnss++;
            spawn->referencesTo++;
            spawnTime = rand()%4+1;
        }
    }
}