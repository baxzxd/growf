#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render.h"
#include "ObjMain.h"
#include "ObjPhys.h"
#include "StandardObjs.h"

float Obj_SqrDist(Gobj *objA, Gobj *objB) {
    V2 c1,c2;
    Obj_GetGlobalCenter(objA, &c1);
    Obj_GetGlobalCenter(objB, &c2);
    return (c2-c1).Len();
}
void Obj_MoveTo(Gobj *objA, Gobj* objB, float v) {
    V2 c1,c2;
    Obj_GetGlobalCenter(objA, &c1);
    Obj_GetGlobalCenter(objB, &c2);
    V2 dir = (c2 - c1).Norm();
    objA->vel = objA->vel + dir * v * del;
}
int Weapon_Update() {
    if( !Obj_HasFlag(o, IN_WORLD) )
        return 0;
    return 0;
}

int Hopper_Update() {
    for( int i = 0; i < maxObjects; i++ ) {
        if( &objects[i] == o || !Obj_HasFlag(&objects[i], IN_WORLD))
            continue;
        
        if( Obj_HasFlag(&objects[i], LIVING))
            continue;
        
        float d = Obj_SqrDist(o, &objects[i]);
        if( d < 160 ) {
            objects[i].energy = 0;
            Obj_MoveTo(&objects[i], o, 140.0f);
            if( d < 48 ) {
                Obj_GiveHealth(&objects[i], -objects[i].health);

                o->energy += 10;
                if( o->energy > 100 )
                    o->energy = 100;
            }
        }
    }
    return 0;
}
void Hopper_Init() {}
int Hopper_Render() {
    float energyRatio = (float)o->energy/100.0f;
    SDL_Rect r;
    r.w = (int)(100.0f * energyRatio);
    r.h = 16;
    r.x = (int)o->pos.x;
    r.y = (int)o->pos.y;
    
    //Render_SetDrawColor(Color_LerpColor(orangeStart,orangeEnd, energyRatio), 255);
    SDL_RenderFillRect(renderer, &r);

    
    return 0;
}
int Box_Update() {
    if( o->immunity )
        return 0;
    if(c.overlap) {
        bool overlap = false;
        /// if not same team damage else health or bullet back
        for( int i = 0; i < c.overlap; i++ ) {
            Gobj *obj = c.overlaps[i];
            if(obj == o || o->parent && (obj == o->parent || obj->parent == o->parent )  )
                continue;
            std::cout<<o<<"collide with"<<obj<<std::endl;
            overlap = true;
            if( o->energy < 0 ) {
                Obj_GiveHealth(obj, -1);
                Obj_Collide(o,obj);
            }
            else {
                Obj_GiveHealth(obj, 1);
            }
        }
        if( overlap ) {
            Obj_GiveHealth(o, -o->health);
        }
    }

    return 0;
}

// trees chopped turn into box object "log" for placing against tilemap?


void Obj_DoTest(bool(*condition), void(*test) ) {
    for( int i = 0; i < maxObjects; i++ ) {
        Gobj *other = &objects[i];
        if( condition )
            continue;
        //test();
    }
}