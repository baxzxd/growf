#include "ObjMain.h"
const int MAX_OVERLAPS = 16;
struct CollInfo {
    Gobj *overlaps[MAX_OVERLAPS];
    int overlap = 0;
};
extern CollInfo c;
void Obj_Collide(Gobj *objA, Gobj *objB);
bool doOverlap(V2 l1, V2 r1, V2 l2, V2 r2);
bool doOverlap(Gobj *objA, Gobj *objB);
bool doOverlap(SDL_Rect *rect, Gobj *obj);
void Obj_GetOverlaps(SDL_Rect *rect, CollInfo *c);
void Phys_Bounds();
void Obj_Physics(float del);