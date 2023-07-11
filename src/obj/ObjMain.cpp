
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

*/
#include <iostream>
#include <algorithm>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render.h"
#include "ObjMain.h"
#include "ObjUtil.h"
#include "ObjPhys.h"
#include "StandardObjs.h"

using namespace std;

Gobj *o;
GobjData *oData;
Gobj *playerObj;
Gobj *selObj;

bool playerHasObj = false;
int totalObjects;
Gobj objects[maxObjects];

float RandRange(int w, int d) {
    return ((rand()%(w*2)) - w)/d;
}

void World_ScatterObj(int id, int amount) {
    for( int i = 0; i < amount; i++ ) {            
        Gobj *tree = Obj_Create(id, {(float)(rand()%SCREEN_WIDTH),(float)(rand()%SCREEN_HEIGHT)}, 1);
    }
}
void Obj_Init() {
    for( int i = 0; i < maxObjects; i++ ) {
        objects[i].data = &objData[3];
        objects[i].health = -1;
    }

    playerObj = Obj_Create(0,{200.0f,200.0f},1);
    Obj_SetFlag(playerObj, LIVING, true);

    Gobj *hopper = Obj_Create(4, {(float)(rand()%SCREEN_WIDTH),(float)(rand()%SCREEN_HEIGHT)}, 1);
    World_ScatterObj(1, 7);
    World_ScatterObj(6, 2);


}            

// display angled objects as 2 rects?
void Obj_Render(Gobj *obj) {   
    V2 mid;
    Obj_GetGlobalCenter(obj, &mid);
    squareRect.x = (int)obj->pos.x;
    squareRect.y = (int)obj->pos.y;
    if( Obj_HasFlag(obj, SHADOW) ) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 96);
        
        squareRect.w = 48;
        squareRect.h = (int)(obj->data->size.x * obj->scale);

        SDL_RenderFillRect(renderer, &squareRect);
    }

    V2 size = Obj_GetSize(obj);
    squareRect.h = (int)size.y;
    squareRect.w = (int)size.x;
    Render_SetDrawColor(Color_Brightness(obj->data->color, .5f), 0xFF);
    SDL_RenderFillRect(renderer, &squareRect);
    
    float healthRatio = ((float)obj->health/(float)obj->data->maxHealth);
    if( healthRatio > 1 )
        healthRatio = 1;
    V2 s = size * healthRatio;
    squareRect.w = (int)s.x; 
    squareRect.h = (int)s.y; 

    V2 d = (size - s)/2;
    squareRect.x = (int)(obj->pos.x + d.x);
    squareRect.y = (int)(obj->pos.y + d.y);
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
        Obj_Render(o);
        if( oData->funcs->funcRender )
            oData->funcs->funcRender();
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
float spawnTime = 0;
float spawnTimer = 0;
Gobj *spawns[16];
int spawnss = 0;
void Obj_Tick() {
    
    for( int i = 0; i < 16; i++ ) {
        if( spawns[i] && spawns[i]->health == -1 ) {
            std::cout<<"dog spawn died"<<std::endl;
            spawns[i]->reserved = false;
            spawns[i] = 0;
            spawnss--;
        }
    }
    if( spawnss < 5 ) {
        spawnTime -= del;
        if( spawnTime <= 0 ) {
            V2 pos;
            pos = {(float)(rand()%SCREEN_WIDTH), 0.0f};
            
            Gobj *spawn = Obj_Create(2,pos,RandV2(100),2);
            spawn->team = 1;

            int r = -1;
            for( int i = 0; i < 16; i++ ) {
                if( !spawns[i] ) {
                    r = i;
                    break;
                }
            }   
            spawns[r] = spawn;
            spawnss++;
            spawn->reserved = true;

            spawnTime = rand()%4+1;
        }
    }

    for( int i = 0; i < maxObjects; i++ ) {
        Tick(&objects[i]);
    }

    if( selObj ) {
        Render_SetDrawColor(Color_RGBToInt(255,255,255), 255);
        
        SDL_Rect r;
        SDL_RenderDrawRect(renderer, Obj_GetRect(selObj, &r));

        Render_String(selObj->data->id + ":" + std::to_string(selObj->health), {(float)r.x,(float)r.y}, 12,15);
    }
}
void Obj_Collide(Gobj *obj, Gobj *collObj) {
    V2 mid = (obj->pos + collObj->pos)/2;
    V2 size = Obj_GetSize(obj) * .5f;
    V2 collSize = Obj_GetSize(collObj);
    float mass = size.x * size.y; // scale mass mayb idk its not that strong
    float collMass = collSize.x * collSize.y;
    collObj->vel = o->vel - (o->vel * mass) / collMass;
    std::cout<<"m:"<<mass<<"collMass:"<<collMass<<std::endl;
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
    if( obj->parent ) {
        Obj_CheckChild(obj->parent, obj);
    }
    //Obj_FillArrayHole();
    totalObjects -= 1;
}