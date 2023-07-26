#include <iostream>
#include <algorithm>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render/render.h"
#include "ObjMain.h"
#include "ObjUtil.h"
#include "ObjPhys.h"

void Obj_GetUsableDistance(Gobj *obj){

}
/*
    parenting

    allow children to float freely in parent for containers
*/
void Obj_ThrowChild( Gobj *obj, int index, V2 aim ) {
    Gobj_Child ch = obj->children[index];
    Child_GiveBond(obj, index, -ch.bond);
    ch.o->vel = aim * 800;
    ch.o->energy = -50;
    ch.o->immunity = 0;
}
bool Obj_AddChild(Gobj *obj, Gobj *child, int bond, V2 pos, bool visible) {
    if( obj->childCount == 8 )
        return false;
    if( Obj_GetPointed(child, OBJPARENT) ) {
        Obj_RemoveChild(Obj_GetPointed(child, OBJPARENT), child);
    }
    
    int emptyIndex = -1;
    for( int i = 0; i < CHILD_COUNT; i++ ) {
        if( obj->children[i].inactive )
            emptyIndex = i;
    }

    // probably not needed if childcount is a thing
    if( emptyIndex == -1 )
        return false;

    Gobj_Child *ch = &obj->children[emptyIndex];
    ch->o = child;
    ch->bond = bond;
    ch->pos = pos;
    ch->inactive = false;
    ch->visible = visible;

    child->held = true;
    Obj_SetPointed(child, obj, OBJPARENT);
    obj->childCount += 1;
    return true;
}
void Obj_RemoveChild(Gobj *obj, int c) {
    Gobj *child = obj->children[c].o;
    child->immunity = .8f;
    child->held = false;
    Obj_SetPointed(child, 0, OBJPARENT);
    obj->children[c].inactive = true;   
    obj->childCount -= 1;
}
void Obj_RemoveChild(Gobj *obj, Gobj *child) {
    for( int i = 0; i < CHILD_COUNT; i++ ) {
        if( !obj->children[i].inactive && obj->children[i].o == child ) {
            return Obj_RemoveChild(obj, i);
            return;
        }
    }
}
void Obj_CheckChild(Gobj *obj, Gobj *child) {
    for( int i = 0; i < CHILD_COUNT; i++ ) {
        if( obj->children[i].o == child ) {
            Obj_RemoveChild(obj, i);
            break; 
        }
    }
}
/// outputs to x and y
V2 Obj_GetCameraPos(Gobj *obj) {
    return obj->pos + cameraPos;
}
V2 Obj_GetCenter(Gobj *obj) {
    return (obj->data->size * obj->scale)/2.0f;
}
V2 Obj_GetGlobalCenter(Gobj *obj) {
    V2 outPos = Obj_GetCenter(obj);
    return obj->pos + outPos;
}
V2 Obj_GetCameraCenter(Gobj *obj) {
    return Obj_GetGlobalCenter(obj) + cameraPos;
}
bool Obj_HasFlag(Gobj *o, Gobj_Flags flags) {
    return o->flags[(int)flags];
}
void Obj_SetFlag(Gobj *obj, Gobj_Flags flag, bool s) {
    obj->flags[(int)flag] = s;
}
V2 Obj_GetSize(Gobj *obj) {
    V2Int v = obj->data->size * obj->scale;
    if( obj->rotation == 270 ) {
        return {-(float)v.y,(float)v.x};
    }
    else if( obj->rotation == 180 ) {
        return {-(float)v.x,(float)v.y};
    }
    else if( obj->rotation == 90 ) {
        return {(float)v.y,(float)v.x};
    }
    return {(float)v.x,(float)v.y};
}
void Obj_AddScale(Gobj *obj, float sc) {
    obj->scale += sc;
    obj->size = Obj_GetSize(obj);
}
void Obj_Rotate(Gobj *obj, int d) {
    obj->rotation = (obj->rotation + d) % 360;
    obj->size = Obj_GetSize(obj);
}
SDL_Rect* Obj_GetRect(Gobj *obj, SDL_Rect *r) {
    V2 s = Obj_GetSize(obj);
    r->x = (int)obj->pos.x;
    r->y = (int)obj->pos.y;
    r->w = (int)s.x;
    r->h = (int)s.y;
    return r;
}
Gobj* Obj_CheckAtMouse() {
    // mayb move to mousePos
    for( int i = 0; i < maxObjects; i++ ) {
        if( !Obj_HasFlag(&objects[i], IN_WORLD))// || selObj == &objects[i] )
            continue;
        V2 d = (mousePos - objects[i].pos);
        float l = d.Len();
        if( l < 16 ) {
            return &objects[i];
        }
    }
    return 0;
}
void Obj_SetTimer(Gobj *obj, float v, Gobj_Timers timer) {
    obj->timers[timer*2] = v;
    obj->timers[timer] = 0;
}
bool Obj_TickTimer(Gobj *obj, Gobj_Timers timer) {
    obj->timers[timer] += del;
    if( obj->timers[timer] > obj->timers[timer*2] ) {
        obj->timers[timer] = 0;
        return true;
    }
    return false;
}
bool Obj_TickTimer(Gobj *obj, Gobj_Timers timer, float v) {
    obj->timers[timer] += del;
    if( obj->timers[timer] > v ) {
        obj->timers[timer] = 0;
        return true;
    }
    return false;
}

Gobj* Obj_Create(std::string id, V2 pos, float sc);
Gobj* Obj_Create(std::string id, V2 pos, V2 vel, float sc) {
    Gobj* obj = Obj_Create(id, pos, sc);
    if( !obj )
        return 0;
    obj->vel = vel;
    return obj;
}
Gobj* Obj_Create(std::string id, V2 pos, float sc) {
    //find empty object
    int emptyIndex = -1;
    for( int i = 0; i < maxObjects; i++ ) {
        if( objects[i].health == -1 && !objects[i].referencesTo ) {
            emptyIndex = i;
            break;
        }
    }
    if( emptyIndex == -1 )
        return 0;
    
    //std::cout<<"CREATED OBJ AT INDEX "<<emptyIndex<<":"<<objects[emptyIndex].held<<std::endl;
    Gobj *obj = &objects[emptyIndex];
    obj->id = id;
    obj->data = &objData[id];
    obj->pos = pos;
    obj->scale = sc;
    obj->size = Obj_GetSize(obj);
    obj->health = obj->data->maxHealth;
    obj->immunity = 0;

    Obj_SetTimer(obj, 2, TIMERGROWTH);
    
    obj->reserved = false;
    obj->held = false;
    obj->childCount = 0;
    memset(&obj->pointers, 0, sizeof( obj->pointers));
    memset(&obj->flags, 0, sizeof( obj->flags ));
    Obj_SetFlag(obj, IN_WORLD, true);
    Obj_SetFlag(obj, JUST_CREATED, true);

    /// maybe has to with del?
    totalObjects += 1;
    return obj;
}

Gobj *Obj_GetPointed(Gobj *obj, OBJ_POINTERS p) {
    return obj->pointers[p];
}
Gobj *Obj_SetPointed(Gobj *obj, Gobj *other, OBJ_POINTERS t) {
    if( obj->pointers[t] ) {
        obj->pointers[t]->referencesTo += -1;
    }
    obj->pointers[t] = other;
    if( other ) {
        obj->pointers[t]->referencesTo += 1;
    }
    return 0;
}