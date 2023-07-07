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

void Animal_Wanderer() {
    for( int i = 0; i < maxObjects; i++ ) {
        Gobj *other = &objects[i];
        if( other == o || !Obj_HasFlag(other, IN_WORLD) )
            continue;
        float d = (objects[i].pos - o->pos).Len();
        if( d > 24*24 ) 
            continue;
        if( o->id != other->id ) {
            if( Obj_HasFlag(other,LIVING))
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
void Animal_Follower() {
    // change dist if parent attacking
    if( o->target ) {
        Obj_MoveTo(o,o->target, 150);
    }
    else if( o->parent->target ) {
        float targetDist = Obj_SqrDist(o,o->parent->target);
        if( targetDist < 50 ) {
            Obj_MoveTo(o, o->parent->target, 95.0f);
            if( targetDist < 20 ) {
                Obj_GiveHealth(o->parent->target, -1);
                o->parent->target->vel = o->parent->target->vel + (o->parent->target->pos-o->pos).Norm() * 100;

                
                // add timer
            }
        }
        else {
            if( Obj_SqrDist(o, o->parent) > 60 ) {
                Obj_MoveTo(o,o->parent, 95.0f);
            }
        }
    }
    else if( Obj_SqrDist(o, o->parent) > 60 ) {
        Obj_MoveTo(o,o->parent, 95.0f);
    }
}
int Animal_Update() {
    bool m = false;
    if( o->parent ) {
        Animal_Follower();
    }
    else {
        if( o->target ) {
            Obj_MoveTo(o, o->target, 95.0f);
        }
        else
            Animal_Wanderer();
    }
    return 0;
}