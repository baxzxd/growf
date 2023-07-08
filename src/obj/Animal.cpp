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

int Animal_Update() {
    bool m = false;
    if( o->parent ) {
        Animal_Follower();
    }
    else {
        if( o->target ) {

            float targetDist = Obj_SqrDist(o,o->target);
            if( targetDist < 24 ) {
                Obj_GiveHealth(o->target, -1);
                o->target->vel = o->target->vel + (o->target->pos-o->pos).Norm() * 100;

            }
            else {
                Obj_MoveTo(o, o->target, 95.0f);
            }
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
        if( d > 24*24 ) 
            continue;

        if( o->id != other->id ) {
            if( Obj_HasFlag(other,LIVING) && o->team != other->team )
                o->target = other;
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
        V2 v ={(float)(rand() & 100 - 50), (float)(rand() & 100 - 50)};
        o->vel = o->vel + v;
    }
}
void MoveToAndAttack(Gobj *obj, Gobj *t, float speed, float minDist, float attackDist) {
    float targetDist = Obj_SqrDist(obj,t);
    if( targetDist < minDist ) {
        Obj_MoveTo(obj, t, speed);
        if( targetDist < attackDist ) {
            Obj_GiveHealth(t, -1);
            t->vel = t->vel + (t->pos-obj->pos).Norm() * 100;

            
            // add timer
        }
    }
}
void Animal_Follower() {
    // change dist if parent attacking
    if( o->target ) {
        MoveToAndAttack(o, o->target, 95.0f, 24, 100);
    }
    else if( o->parent->target ) {
        MoveToAndAttack(o, o->parent->target, 95.0f, 24, 50);
    }
    else if( Obj_SqrDist(o, o->parent) > 60 ) {
        Obj_MoveTo(o,o->parent, 95.0f);
    }
}