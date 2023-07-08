#ifndef OBJ_UTIL
#define OBJ_UTIL
#include "ObjMain.h"
#include "../main.h"
#include <SDL2/SDL.h>

typedef enum {
    TIMERGROWTH = 0,
    TIMERDECAY = 1
} Gobj_Timers;
typedef enum {
    OBJPARENT = 0,
    OBJTARGET = 1
} OBJ_POINTERS;

Gobj *Obj_GetPointed(Gobj *obj, OBJ_POINTERS p);
Gobj *Obj_SetPointed(Gobj *obj, Gobj *other, OBJ_POINTERS t);

Gobj* Obj_Create(int id, V2 pos, float sc);
Gobj* Obj_Create(int id, V2 pos, V2 vel, float sc);
V2 Obj_GetSize(Gobj *obj);
bool  Obj_HasFlag(Gobj *o, Gobj_Flags flag);
void Obj_SetFlag(Gobj *o, Gobj_Flags flag, bool s);
float Obj_SqrDist(Gobj *objA, Gobj *objB);
void Obj_AddChild(Gobj *obj, Gobj *child, int bond, V2 pos);
void Obj_RemoveChild(Gobj *obj, int i);
void Obj_CheckChild(Gobj *obj, Gobj *child);
SDL_Rect *Obj_GetRect(Gobj *obj, SDL_Rect *r);
#endif