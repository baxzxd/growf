
#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render/render.h"
#include "../world/world.h"
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
       if( Obj_HasFlag(&objects[i], IN_WORLD) && doOverlap(rect, &objects[i]) ){
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
            Obj_HasFlag(&objects[i], NO_COLLISION)  )
            continue;
        if( doOverlap(obj, &objects[i]) ){
            //Obj_Collide(o, &objects[i]);
            c->overlaps[c->overlap] = &objects[i];
            c->overlap += 1;

            if( Obj_HasFlag( &objects[i], STATIC )) {
                
            }
        }
    }

}

void World_Unstuckify(Gobj *obj) {

    int tryCount = 0;
    int tX,tY;
    V2 oPos = o->pos / tileSize;
    // check in growing radius?
    for( int x = -1; x < 2; x++ ) {
        for( int y = -1; y < 2; y++ ) {
            //WorldTile *tryTile = &tiles[(int)oPos.x + x][(int)oPos.y + y];
            //if( !tryTile->height || (x==1 && y==1))
            //   o->pos = tryTile->pos;
        }
    }
}
void Obj_TileColl(Gobj *obj) {
    
}
// get overlaps at the same time and find pairs?
void Obj_Physics(float del) {
    if( !o->held && !Obj_HasFlag(o, STATIC)) {
        int colls = 0;
        V2 oldPos = o->pos;
        //check y
        V2 vD = o->vel * del;
        o->pos = o->pos + V2{0,vD.y}; 
        WorldTile *coll = World_CheckToMap(o);
        if( coll && coll->data ) {
            Obj_TileColl(o);
            o->pos = oldPos;
            o->vel.y = 0;
            colls += 1;
        }
        oldPos = o->pos;
        o->pos = o->pos + V2{vD.x,0}; 
        coll = World_CheckToMap(o);
        if( coll && coll->data ) {
            if( !colls )
                Obj_TileColl(o);      ////////obj coll struct instead with enum for tile or obj coll

            o->pos = oldPos;
            o->vel.x = 0;
            colls += 1;

            coll = World_CheckToMap(o);
            if( coll && coll->data )
                World_Unstuckify(o);
        }

        Friction(&o->vel);
        Obj_GetOverlaps(o, &c);
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
SDL_Rect result;
bool doOverlap(SDL_Rect *rect, Gobj *obj) {
    overlapRect.x = (int)obj->pos.x;
    overlapRect.y = (int)obj->pos.y;
    overlapRect.w = (int)obj->size.x;
    overlapRect.h = (int)obj->size.y;

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