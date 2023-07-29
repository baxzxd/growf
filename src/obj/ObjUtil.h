#ifndef OBJ_UTIL
#define OBJ_UTIL
#include "ObjMain.h"
#include "../main.h"
#include <SDL2/SDL.h>
#include <string>

typedef enum {
    TIMERGROWTH = 0,
    TIMERDECAY = 1,
    TIMERTARGET = 2,
    TIMERMISC = 3,
    OBJ_TIMER0 = 6,
    OBJ_TIMER1 = 7,
    OBJ_TIMER2 = 8
} Gobj_Timers;
typedef enum {
    OBJPARENT = 0,
    OBJTARGET = 1
} OBJ_POINTERS;
void Obj_ThrowChild( Gobj *obj, int index, V2 aim );
Gobj *Obj_GetPointed(Gobj *obj, OBJ_POINTERS p);
Gobj *Obj_SetPointed(Gobj *obj, Gobj *other, OBJ_POINTERS t);
void Obj_SetTimer(Gobj *obj, float v, Gobj_Timers timer);
bool Obj_TickTimer(Gobj *obj, Gobj_Timers timer);
bool Obj_TickTimer(Gobj *obj, Gobj_Timers timer, float v);
Gobj* Obj_Create(std::string id, V2 pos, float sc);
Gobj* Obj_Create(std::string id, V2 pos, V2 vel, float sc);
V2 Obj_GetSize(Gobj *obj);
V2 Obj_GetCenter(Gobj *obj);
V2 Obj_GetGlobalCenter(Gobj *obj);
V2 Obj_GetCameraPos(Gobj *obj);
V2 Obj_GetCameraCenter(Gobj *obj);
V2 Obj_GetCameraPos(Gobj *obj);
void Obj_AddScale(Gobj *obj, float sc);
void Obj_Rotate(Gobj *obj, int r);
bool  Obj_HasFlag(Gobj *o, Gobj_Flags flag);
void Obj_SetFlag(Gobj *o, Gobj_Flags flag, bool s);
float Obj_SqrDist(Gobj *objA, Gobj *objB);
bool Obj_AddChild(Gobj *obj, Gobj *child, int bond, V2 pos, bool visible);
void Obj_RemoveChild(Gobj *obj, int i);
void Obj_RemoveChild(Gobj *obj, Gobj *child);
void Obj_CheckChild(Gobj *obj, Gobj *child);
SDL_Rect *Obj_GetRect(Gobj *obj, SDL_Rect *r);
#endif