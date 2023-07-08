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

void Obj_GetUsableDistance(Gobj *obj){

}
/*
    parenting

    allow children to float freely in parent for containers
*/
void Obj_AddChild(Gobj *obj, Gobj *child, int bond, V2 pos) {
    if( child->parent || obj->childCount == 8 )
        return;

    
    int emptyIndex = -1;
    for( int i = 0; i < CHILD_COUNT; i++ ) {
        if( obj->children[i].inactive )
            emptyIndex = i;
    }
    if( emptyIndex == -1 )
        return;

    Gobj_Child *ch = &obj->children[emptyIndex];
    ch->o = child;
    ch->bond = bond;
    ch->pos = pos;
    ch->inactive = false;
    child->held = true;
    child->parent = obj;
    obj->childCount += 1;
}
void Obj_RemoveChild(Gobj *obj, int c) {
    Gobj *child = obj->children[c].o;
    child->immunity = .8f;
    child->held = false;
    obj->children[c].inactive = true;   
    obj->childCount -= 1;
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
void Obj_GetCenter(Gobj *obj, V2 *outPos) {
    *outPos = (obj->data->size * obj->scale)/2.0f;
}
void Obj_GetGlobalCenter(Gobj *obj, V2 *outPos) {
    Obj_GetCenter(obj, outPos);
    *outPos = obj->pos + *outPos;
}
bool Obj_HasFlag(Gobj *o, Gobj_Flags flags) {
    return o->flags[(int)flags];
}
void Obj_SetFlag(Gobj *obj, Gobj_Flags flag, bool s) {
    obj->flags[(int)flag] = s;
}
V2 Obj_GetSize(Gobj *obj) {
    V2Int v = obj->data->size * obj->scale;
    return {(float)v.x,(float)v.y};
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
    SDL_Rect mouseR;
    mouseR.x = mousePos.x;
    mouseR.y = mousePos.y;
    mouseR.w = 4;
    mouseR.h = 4;

    for( int i = 0; i < maxObjects; i++ ) {
        if( !Obj_HasFlag(&objects[i], IN_WORLD) || selObj == &objects[i] )
            continue;
        V2 d = (mousePos - objects[i].pos);
        float l = d.Len();
        if( l < 16 ) {
            std::cout<<"obj sel"<<std::endl;
            return &objects[i];
        }
    }
    return 0;
}

Gobj* Obj_Create(int id, V2 pos, float sc);
Gobj* Obj_Create(int i, V2 pos, V2 vel, float sc) {
    Gobj* obj = Obj_Create(i, pos, sc);
    if( !obj )
        return 0;
    obj->vel = vel;
    return obj;
}
Gobj* Obj_Create(int id, V2 pos, float sc) {
    //find empty object
    int emptyIndex = -1;
    for( int i = 0; i < maxObjects; i++ ) {
        if( objects[i].health == -1 && !objects[i].reserved ) {
            emptyIndex = i;
            break;
        }
    }
    if( emptyIndex == -1 )
        return 0;
    
    std::cout<<"CREATED OBJ AT INDEX "<<emptyIndex<<":"<<objects[emptyIndex].held<<std::endl;
    Gobj *obj = &objects[emptyIndex];
    obj->id = id;
    obj->data = &objData[id];
    obj->pos = pos;
    obj->scale = sc;
    obj->health = obj->data->maxHealth;


    obj->timers[0] = 2;
    obj->timers[1] = 2;
    
    obj->reserved = false;
    obj->held = false;
    obj->parent = 0;
    obj->childCount = 0;

    memset(&obj->flags, 0, sizeof( obj->flags ));
    Obj_SetFlag(obj, IN_WORLD, true);
    Obj_SetFlag(obj, JUST_CREATED, true);

    /// maybe has to with del?
    totalObjects += 1;
    return obj;
}

Gobj *Obj_GetPointed(Gobj *obj, OBJ_POINTERS p) {
    return 0;
}
Gobj *Obj_SetPointed(Gobj *obj, Gobj *other, OBJ_POINTERS t) {
    if( obj->pointers[t] ) {
        obj->pointers[t]->referencesTo--;
    }
    obj->pointers[t] = other;
    return 0;
}