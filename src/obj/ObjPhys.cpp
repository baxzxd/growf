
#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render.h"
#include "ObjMain.h"
#include "ObjUtil.h"
#include "ObjPhys.h"
using namespace std;

CollInfo c;
float friction = 0.96f;
float tempFriction;
void Friction(V2 *v) {
    V2 a = {v->x,v->y};
    V2 fric = a * friction * del;
    a = a - fric;  
    *v = a; 
} 
void Obj_GetOverlaps(SDL_Rect *rect, CollInfo *c) {
    c->overlap = 0;
    for( int i = 0; i < maxObjects; i++ ) {
       if( doOverlap(rect, &objects[i]) ){
            //Obj_Collide(o, &objects[i]);
            c->overlaps[c->overlap] = &objects[i];
            c->overlap += 1;
        }
    }
}
void Obj_GetOverlaps(Gobj *obj, CollInfo *c) {
    c->overlap = 0;
    for( int i = 0; i < maxObjects; i++ ) {
        if(&objects[i] == obj || !Obj_HasFlag(&objects[i], IN_WORLD) || 
            Obj_HasFlag(&objects[i], NO_COLLISION) || (obj->immunity > 0 || objects[i].immunity > 0) )
            continue;
        if( doOverlap(obj, &objects[i]) ){
            //Obj_Collide(o, &objects[i]);
            c->overlaps[c->overlap] = &objects[i];
            c->overlap += 1;
        }
    }

}
void Obj_Physics(float del) {
    Obj_GetOverlaps(o, &c);
    if( !o->held && !Obj_HasFlag(o, STATIC)) {
        o->pos = o->pos + o->vel * del;
        Friction(&o->vel);
        Phys_Bounds();
    }
}

void Phys_Bounds() {
    float bounce = .2f;
    // lower bound
    if( o->pos.x <= 0 ) {
        o->vel.x *= -bounce;
        o->pos.x = 1;
    }
    if( o->pos.y <= 0 ) {
        o->vel.y *= -bounce;            
        o->pos.y = 1;
    }

    //upper bound
    if( o->pos.x >= SCREEN_WIDTH ) {
        o->vel.x *= -1;
        o->pos.x = SCREEN_WIDTH - 1;
    }
    if( o->pos.y >= SCREEN_HEIGHT ) {
        o->vel.y *= -1;            
        o->pos.y = SCREEN_HEIGHT - 1;
    }
}

// Returns true if two rectangles (l1, r1) and (l2, r2)
// overlap
SDL_Rect overlapRect;
bool doOverlap(SDL_Rect *rect, Gobj *obj) {
    V2 size = Obj_GetSize(obj);
    overlapRect.x = (int)obj->pos.x;
    overlapRect.y = (int)obj->pos.y;
    overlapRect.w = (int)size.x;
    overlapRect.h = (int)size.y;

    SDL_Rect result;
    SDL_bool b = SDL_IntersectRect(rect, &overlapRect, &result);
    return (bool)b;
}
bool doOverlap(Gobj *objA, Gobj *objB) {
    V2 l1 = objA->pos;
    V2 r1 = objA->pos + Obj_GetSize(objA);
    V2 l2 = objB->pos;
    V2 r2 = objB->pos + Obj_GetSize(objB);
    return doOverlap(l1,r1,l2,r2);
}
bool doOverlap(V2 l1, V2 r1, V2 l2, V2 r2){//, ENUMOVERLAP *overlap, int *overlapX, int *overlapY) {
    // if rectangle has area 0, no overlap
    if (l1.x == r1.x || l1.y == r1.y || r2.x == l2.x || l2.y == r2.y)
        return false;
   
    // If one rectangle is on left side of other
    if (l1.x > r2.x || l2.x > r1.x)
        return false;
 
    // If one rectangle is above other
    if (r1.y < l2.y || r2.y < l1.y)
        return false;

    ENUMOVERLAP overlap;
    int overlapX, overlapY;
    /*
    if( l1x < r1x ) {
        // above
        if( l1y < r1y ) {
            overlap = ENUMOVERLAP::TL;
        }
        else {
            overlap =ENUMOVERLAP::BL;
        }
    }
    else {
        if( l1y < r1y ) {
            overlap = ENUMOVERLAP::TR;
        }
        else {
            overlap = ENUMOVERLAP::BR;
        }
    }
    switch(overlap) {
        case ENUMOVERLAP::TL:
            overlapX = r1x - l1x;
            overlapY = r1y - l1y;
        break;

        case ENUMOVERLAP::TR:
            overlapX = r1x - r2x;
            overlapY = r1y - l1y;
        break;
    }*/

    return true;
}