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
#include "StandardObjs.h"
#include "Animal.h"

void MoveToAndAttack(Gobj *obj, Gobj *t, float speed, float minDist, float attackDist) {
    float targetDist = Obj_SqrDist(obj,t);
    if( targetDist < minDist ) {
        Obj_MoveTo(obj, t, speed);
        if( targetDist < attackDist ) {
            Obj_GiveHealth(t, -1);
            t->vel = t->vel + (t->pos-obj->pos).Norm() * 100;
            t->immunity = .4f;
            
            // add timer
        }
    }
}
int Animal_Update() {
    bool m = false;
    if( o->parent ) {
        Animal_Follower();
    }
    else {
        if( o->target ) {
            MoveToAndAttack(o, o->target, 95.0f, 100, 24);
        }
        else
            Animal_Wanderer();
    }
    return 0;
}
void Animal_Wanderer() {
    for( int i = 0; i < maxObjects; i++ ) {
        Gobj *other = &objects[i];
        if( other == o || !Obj_HasFlag(other, IN_WORLD) )
            continue;
        
        float d = (objects[i].pos - o->pos).Len();
        if( d > 150 ) 
            continue;

            //just reposition array once per removal getting last element bsaed on count or element before count if at end

        if( o->id != other->id ) {
            if( Obj_HasFlag(other,LIVING) && o->team != other->team ) {
//                o->target = other;

            }
        }
        else {
            if( objects[i].parent ) {
            // o->target = objects[i].parent;
            }
            else {
                // check whos more naturally aggressive
                objects[i].parent = o;
            }
            //parenting sets mate? if potential mates parent isnt null mayb target?
        }
    }
    o->timers[5] += del;
    if( o->timers[5] > 3 ) {
        o->timers[5] = 0;
        o->vel = o->vel + RandV2(200);
    }
}
void Animal_Follower() {
    // change dist if parent attacking
    if( o->target ) {
        MoveToAndAttack(o, o->target, 95.0f, 100, 24);
    }
    else if( o->parent->target ) {
        MoveToAndAttack(o, o->parent->target, 95.0f, 100, 24);
    }
    else if( Obj_SqrDist(o, o->parent) > 60 ) {
        Obj_MoveTo(o,o->parent, 95.0f);
    }
}