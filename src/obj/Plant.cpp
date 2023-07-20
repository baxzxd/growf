#include <iostream>
#include <algorithm>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render.h"
#include "ObjMain.h"
#include "ObjUtil.h"
#include "ObjPhys.h"
#include "StandardObjs.h"

int Plant_Init() {
    Obj_SetTimer(o, 1, TIMERGROWTH);
    return 0;
}
// use tilemap to find water through roots per tick

// hoe item to draw surface onto map
// surface resizes depending on how far stuff is drawn, liquid filling? irrigation idk
// tile struct that stores if it was dug on or amount of material left?

int Plant_Use(Gobj *obj) {
    Obj_SetFlag(obj, LIVING, true);
    Obj_SetFlag(obj, STATIC, true);

    return 0;
}
int Plant_Death(Gobj *obj) {
    for( int i = 0; i < rand()%3; i++ ) {
        Obj_Create("tree",obj->pos, RandV2(500),.25f);
    }
    Obj_Create("log", obj->pos, 1);
    return 0;

}
//trunk width, leaf start height, side branches?
int trunkHeight = 5;
int trunkWidth = 2;
int leafWidth = 8;
SDL_Rect r;

int Plant_Render() {
    Render_SetDrawColor(Color_RGBToInt(0,0,0),100);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    V2 center = Obj_GetCameraCenter(o);
    r.x = center.x;
    r.y = center.y - (trunkWidth*o->scale)/2;
    r.h = (trunkWidth*o->scale);
    r.w = (trunkHeight*o->scale);
    SDL_RenderFillRect(renderer, &r);
    r.h = leafWidth * o->scale;
    r.w = leafWidth* o->scale;
    r.x = r.x + trunkHeight* o->scale;
    r.y = center.y - (leafWidth* o->scale)/2;
    SDL_RenderFillRect(renderer, &r);
    return 0;
}

void Obj_GiveEnergy(Gobj *obj, int energy) {
    obj->energy += energy;
    if( obj->energy < 0 )
        obj->energy = 0;
}
void Plant_FruitSpawnTick() {
    if( o->childCount == 5  )
        return;
    if( !Obj_TickTimer(o, TIMERGROWTH, 1) ) 
        return;
    Obj_GiveEnergy(o, -25);
    //create fruit
    V2 supPos = {(float)(rand()%(int)(o->size.x)),(float)(rand()%((int)o->size.y))};
    Gobj* fruit = Obj_Create("apple", o->pos,RandV2(100), .2f);
    if( fruit ) {
        fruit->energy = 25;
        fruit->immunity = .6f;
        // green to orange
        //Obj_SetGrowGradient( Color_RGBToInt(0, 165, 0), Color_RGBToInt(255, 165, 0));
        Obj_AddChild(o, fruit, 100, supPos, true);
        //Obj_SetFlag(fruit, LIVING, true);
    }
}
void Plant_FruitGrowthTick() {
    if( !Obj_TickTimer(o, TIMERMISC, .1f) ) 
        return;
    Obj_GiveEnergy(o, -2);
    for( int i = 0; i < CHILD_COUNT; i++ ) {
        if( o->children[i].inactive || (o->children[i].o->size.x > 14 ||o->children[i].o->size.y > 14 ))
            continue;
        Obj_AddScale(o->children[i].o, .1f);
    }
}

int Plant_Update() {
    //draw darker shadow under plant to show health as animals eat it
    //water to grow
    // transparent green particles when bit
    bool tickGrowth = false;
    switch( o->state ) {
        // growing
        case 0:
            if( !Obj_HasFlag(o, LIVING ))
                break;
            if( tickGrowth ) {
                if( Obj_TickTimer(o, TIMERGROWTH, 1) ) {
                    Obj_AddScale(o,1);
                }
            }
            else {
                Obj_AddScale(o, 2.0f*del);
            }
            
            if( o->scale >= 10.0f ) {
                std::cout << "plant done grew" << std::endl;
                o->state = 1;
                Obj_SetTimer(o, 1, TIMERGROWTH);
            }
        break;

        // decaying
        case 1:
            if( o->energy == 0 ) 
                break;
            Plant_FruitSpawnTick();
            Plant_FruitGrowthTick();
        break;

        // dead
        case 2:

        break;
    }
    return 0;
}