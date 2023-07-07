#ifndef OBJ_MAIN
#define OBJ_MAIN
#include <SDL2/SDL.h>

struct Gobj_Func {
    int (*funcInit)();
    int (*funcRender)();
    int (*funcUpdate)();
};
/// id, health, w, h, scale, color, update

struct GobjData{
    std::string id = "";
    /// possibly rename to basescale
    V2Int size;
    // bitshift to get r,g,b?
    int color = 256;
    int maxHealth = 100;
    Gobj_Func *funcs;
    
    GobjData(char i[], int health, V2Int s, int c, Gobj_Func *func) : 
            id(i), maxHealth(health), size(s), color(c), funcs(func){}
};
// obj data
typedef enum {
    GROWS = 0,
    IN_WORLD = 1,
    SHADOW = 2,
    JUST_CREATED = 3,
    TO_RECYCLE = 4,
    LIVING = 5,
    DECAYING = 6,
    NO_COLLISION = 7,
    STATIC = 8
} Gobj_Flags;
typedef enum {
    TIMERGROWTH = 0,
    TIMERDECAY = 1
} Gobj_Timers;
struct Gobj;
struct Gobj_Child {
    Gobj* o;
    int bond;
    V2 pos;
    bool inactive = true;
};
const int CHILD_COUNT = 8;
struct Gobj {
    int id;
    V2 pos;
    V2 cPos;
    V2 vel;
    int health = 10;
    int color = 256;
    int growthStage;
    float growth;
    float scale = 1;
    float immunity = 0;
    int tickArgs[32];
    float timers[8]; // every other index stores actual timer? first stores default value?
    int state = 0;
    bool held;

    int energy; ///used for health loss, moving?
    bool flags[64];

    Gobj *parent;
    Gobj *target;
    GobjData *data;

    int childCount;
    Gobj_Child children[CHILD_COUNT];
};
struct World {
    V2Int size;
    
};

/// where first rect in collision is positioned
typedef enum {
    TL,
    TR,
    BL,
    BR
} ENUMOVERLAP;

struct ObjController {};

extern int totalObjects;
const int maxObjects = 1024;
extern Gobj objects[maxObjects];
extern GobjData objData[5];
extern Gobj_Func objFuncs[8];
extern Gobj *playerObj;
extern Gobj *o;
extern Gobj *selObj;


bool  Obj_HasFlag(Gobj *o, Gobj_Flags flag);
void Obj_SetFlag(Gobj *o, Gobj_Flags flag, bool s);
void Obj_Init();
void Obj_Tick();
V2 Obj_GetSize(Gobj *obj);
void Obj_Render(Gobj *obj);
Gobj* Obj_Create(int id, V2 pos, float sc);
Gobj* Obj_Create(int id, V2 pos, V2 vel, float sc);
void Obj_Death(Gobj *obj);
void Obj_GiveHealth(Gobj *obj, int h);
void Obj_FillArrayHole();
void Obj_GetGlobalCenter(Gobj *obj, V2 *outPos);
void Obj_MoveTo(Gobj *objA, Gobj *objB, float f);
float Obj_SqrDist(Gobj *objA, Gobj *objB);
void Obj_AddChild(Gobj *obj, Gobj *child, int bond, V2 pos);
void Obj_RemoveChild(Gobj *obj, int i);
void Obj_CheckChild(Gobj *obj, Gobj *child);
SDL_Rect *Obj_GetRect(Gobj *obj, SDL_Rect *r);

/// checks if selObj needs updated, or for obj interactions
Gobj* Obj_CheckAtMouse();
#endif