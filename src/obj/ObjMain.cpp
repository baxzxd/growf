
/*x`
csv templates


    growing food sources that can be attacked and infected
    shrink per each food taken
        -cluster of sources as one

    TICK BASED GAMELOOPS
        demos
      ideas
      chemistry
        -leaves can be ground to chlorphyll for power
    
    growing and shrinking bubble in front of baby living objects

      every object can grow?
        -guns get bigger barrels
        -plants and animals duh
        --stages of growth?
            -plants (fruit) -> seed -> plant
                -fruit decays into seed
            -guns barrel grows and can be shaven down?


    ui elements
        -render downward through elements onto surface?
            --layering could be done with lists or arrays or somethin


    issues
    deaths arent being tracked with parenting still?


    randomly generate rocks with speckles and different sizes
        somehow save speckle locations?
        finish inventory to store elements in rocks


    animals guard power source and try to steal it back

    island like perlin noise and smaller "ore" stone chunks for map
    also water n such so irrigation can be added
        painted world with fill material tool


    big knockback on knockout
*/
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render/render.h"
#include "../world/world.h"
#include "ObjMain.h"
#include "ObjUtil.h"
#include "ObjPhys.h"
#include "ObjData.h"

using namespace std;

Gobj *o;
GobjData *oData;
Gobj *playerObj;
Gobj *cameraFocus;

bool playerHasObj = false;
int totalObjects;
Gobj objects[maxObjects];

float RandRange(int w, int d) {
    return ((rand()%(w*2)) - w)/d;
}
void World_ScatterObj( std::string id, int amount ) {
    for( int i = 0; i < amount; i++ ) {            
        Gobj *tree = Obj_Create(id, {(float)(rand()%SCREEN_WIDTH),(float)(rand()%SCREEN_HEIGHT)}, 1);
    }
}
void World_ScatterObj(std::string id, int amount, Gobj* objs[]) {
    for( int i = 0; i < amount; i++ ) {            
        Gobj *tree = Obj_Create(id, {(float)(rand()%SCREEN_WIDTH),(float)(rand()%SCREEN_HEIGHT)}, 1);
        objs[i] = tree;
    }
}
#include "../FastNoiseLite.h"
void Obj_Init() {
    for( int i = 0; i < maxObjects; i++ ) {
        objects[i].data = &objData["tree"];
        objects[i].health = -1;
        
        //[prolly messy]
        objects[i].referencesTo = 0;
        memset(objects[i].flags, 0, sizeof(objects[i].flags));
    }
    totalObjects = 0;
    playerObj = Obj_Create("player",{200.0f,200.0f},1);
    Obj_SetFlag(playerObj, LIVING, true);
    cameraFocus = playerObj;



    Obj_Create("hopper", {(float)(rand()%SCREEN_WIDTH),(float)(rand()%SCREEN_HEIGHT)}, 1);
    World_ScatterObj("stone", 2);
    World_ScatterObj("nail", 5);
}            

// display angled objects as 2 rects?
void Obj_Render(Gobj *obj) {   
    V2 pos = Obj_GetCameraPos(obj);
    squareRect.x = (int)pos.x;
    squareRect.y = (int)pos.y;
    if( Obj_HasFlag(obj, SHADOW) ) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 96);
        
        squareRect.w = 48;
        squareRect.h = (int)(obj->data->size.x * obj->scale);

        SDL_RenderFillRect(renderer, &squareRect);
    }

    squareRect.h = (int)obj->size.y;
    squareRect.w = (int)obj->size.x;
    Render_SetDrawColor(Color_Brightness(obj->data->color, .5f), 0xFF);
    SDL_RenderFillRect(renderer, &squareRect);
    
    float healthRatio = ((float)obj->health/(float)obj->data->maxHealth);
    if( healthRatio > 1 )
        healthRatio = 1;
    V2 s = obj->size * healthRatio;
    squareRect.w = (int)s.x; 
    squareRect.h = (int)s.y; 

    V2 d = (obj->size - s)/2;
    squareRect.x = (int)(pos.x + d.x);
    squareRect.y = (int)(pos.y + d.y);
    Render_SetDrawColor(obj->data->color, 0xFF);
    SDL_RenderFillRect(renderer, &squareRect);
}     

//(OBJNAME)_ChildTick function?
void Tick(Gobj *obj) {
    o = obj;
    oData = o->data;

    if( !Obj_HasFlag(o, IN_WORLD) ) 
        return;
    if( Obj_HasFlag(o, JUST_CREATED) ) {
        if( o->data->funcs->funcInit ) 
            o->data->funcs->funcInit();
        Obj_SetFlag(o,JUST_CREATED, false);
        return;
    }

    for( int i = 0; i < OBJ_POINTER_AMOUNT; i++ ) {
        if( o->pointers[i] && o->pointers[i]->health == -1 )
            o->pointers[i] = 0;
    }

    Obj_Physics(del);

    if( o->immunity > 0 ) {
        o->immunity -= del;
        if( o->immunity <= 0 )
            o->immunity = 0;
    }

    // generic decay func?

    if( oData->funcs->funcUpdate)
        oData->funcs->funcUpdate();
    
    V2 rSize = Obj_GetSize(o);
    o->cPos = o->pos + rSize / 2;
    
    if( !o->held ) {
        //prerender?
        if( oData->funcs->funcRender )
            oData->funcs->funcRender();
        Obj_Render(o);
        if( o->childCount ) {
            for( int i = 0; i < CHILD_COUNT; i++ ) {
                if( o->children[i].inactive )
                    continue;
                o->children[i].o->pos = o->pos + o->children[i].pos;
                if( o->children[i].visible )
                    Obj_Render(o->children[i].o);
            }
        }
    }
}
struct ObjSelection {
    Gobj *obj;
    int color;
    SDL_Rect r;
    bool inactive = true;
};
ObjSelection selections[8];
void Obj_Tick() {
    /*
    for( int x = -1; x < (SCREEN_WIDTH / tileSize)/chunkSize; x++ ) {
        for( int y = -1; y < (SCREEN_HEIGHT / tileSize)/chunkSize; y++ ) {
            noiseDestRect.x = x * tileSize * chunkSize + cameraPos.x;
            noiseDestRect.y = y * tileSize * chunkSize + cameraPos.y;
            if( camX + x > 0 && camY + y > 0 )
                SDL_RenderCopy(renderer,chunkTextures[camX + x][camY + y], NULL, &noiseDestRect);
        }
    }
*/

    for( int i = 0; i < maxObjects; i++ ) {
        Tick(&objects[i]);
    }

    for( int i = 0; i < 8; i++ ) {
        if( selections[i].inactive )
            continue;
    
        Render_SetDrawColor(Color_RGBToInt(255,255,255), 255);
        SDL_RenderDrawRect(renderer, Obj_GetRect(selections[i].obj, &selections[i].r));
        Render_String(selections[i].obj->data->id + ":" + std::to_string(selections[i].obj->health), {(float)selections[i].r.x,(float)selections[i].r.y}, 12,15);
    }
}

//use speckled texture for banana spots
//create multiple textures for speckles
void Obj_Collide(Gobj *obj, Gobj *collObj) {
    V2 mid = (obj->pos + collObj->pos)/2;
    float mass = (obj->size.x * obj->size.y) * .5f; // scale mass mayb idk its not that strong
    float collMass = collObj->size.x * collObj->size.y;
    collObj->vel = collObj->vel + (o->vel * mass) / (collMass*.5f);
}
void Obj_Decay(Gobj *obj) {
    obj->timers[2] -= del;
    if( obj->timers[2] <= 0 ) {
        Obj_GiveHealth(obj, -5);
        obj->timers[2] = obj->timers[3];
    }
}
void Child_GiveBond(Gobj *obj, int child, int b) {
    obj->children[child].bond += b;
    if( obj->children[child].bond <= 0 ) {
        Obj_RemoveChild(obj, child);
    }
}
void Obj_GiveHealth(Gobj *obj, int h) {
    if( h < 0 && obj->immunity ) 
        return;
    obj->health += h;
    if( h < 0 ) {
        for( int i = 0; i < CHILD_COUNT; i++ ) {
            if( obj->children[i].inactive )
                continue;
            Child_GiveBond(obj, i, -50);
        }
    }
    if( obj->health <= 0 ) {
        Obj_Death(obj);
    }
}
void Obj_Death(Gobj *obj) {
    std::cout<<obj->data->id<<" died"<<std::endl;
    if( obj->data->funcs->funcDeath )
        obj->data->funcs->funcDeath(obj);
    Obj_SetFlag(obj, IN_WORLD, false);
    obj->health = -1;
    if( Obj_GetPointed(obj, OBJPARENT) ) {
        Obj_CheckChild(Obj_GetPointed(obj, OBJPARENT), obj);
    }
    //Obj_FillArrayHole();
    totalObjects -= 1;
}