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

int MoveToAndAttack(Gobj *obj, Gobj *t, float speed, float minDist, float attackDist) {
    float targetDist = Obj_SqrDist(obj,t);
    if( targetDist < minDist ) {
        Obj_MoveTo(obj, t, speed);
        if( targetDist < attackDist ) {
            Obj_GiveHealth(t, -1);
            t->vel = t->vel + (t->pos-obj->pos).Norm() * 100;
            t->immunity = .4f;
            return 2;
            // add timer
        }
        return 1;
    }
    return 0;
}
int Animal_Update() {
    bool m = false;
    if( Obj_GetPointed(o, OBJPARENT)) {
        Animal_Follower();
    }
    else {
        if( Obj_GetPointed(o, OBJTARGET) ) {
            if( !MoveToAndAttack(o, Obj_GetPointed(o, OBJTARGET), 95.0f, 250, 24) ) {
                if( Obj_TickTimer(o, TIMERTARGET, 1)) {
                    Obj_SetPointed(o, 0, OBJTARGET);
                }
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
        if( d > 150 ) 
            continue;

            //just reposition array once per removal getting last element bsaed on count or element before count if at end
        /*
        check for possible targets
        check for edible objects and if they have a parent atack it
        */
        if( o->id != other->id ) {
            if( !Obj_HasFlag(other,STATIC) && Obj_HasFlag(other,LIVING) && o->team != other->team ) {
                Obj_SetPointed(o, other, OBJTARGET);
                Obj_SetTimer(o, 1, TIMERTARGET);
            }
        }
        else {
            if( Obj_GetPointed(&objects[i], OBJPARENT) ) {
            // o->target = objects[i].parent;
            }
            else {
                // check whos more naturally aggressive
                Obj_SetPointed(&objects[i], o, OBJPARENT);
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
    Gobj *parent = Obj_GetPointed(o, OBJPARENT);
    if( Obj_GetPointed(o, OBJTARGET) ) {
        if( !MoveToAndAttack(o, Obj_GetPointed(o, OBJTARGET), 95.0f, 200, 24) ) {
            if( Obj_TickTimer(o, TIMERTARGET, 1)) {
                Obj_SetPointed(o, 0, OBJTARGET);
            }
        }
    }
    else if( Obj_GetPointed(parent, OBJTARGET) ) {
        MoveToAndAttack(o, Obj_GetPointed(parent, OBJTARGET), 95.0f, 100, 24);
    }
    else if( Obj_SqrDist(o, parent) > 30 ) {
        Obj_MoveTo(o,parent, 95.0f);
    }
}