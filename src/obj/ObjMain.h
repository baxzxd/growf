#ifndef OBJ_MAIN
#define OBJ_MAIN
#include <SDL2/SDL.h>

struct Gobj;
struct Gobj_Func {
    int (*funcInit)();
    int (*funcRender)();
    int (*funcUpdate)();
    int (*funcUse)(Gobj *obj);
    int (*funcDeath)(Gobj *obj);
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
struct Gobj;
struct Gobj_Child {
    Gobj* o;
    int bond;
    V2 pos;
    bool inactive = true;
    bool visible = true;
};
const int OBJ_POINTER_AMOUNT = 8;
const int CHILD_COUNT = 8;
struct Gobj {
    int id;
    V2 pos;
    V2 cPos;
    V2 vel;
    int team = 0;
    int health = 10;
    int color = 256;
    int growthStage;
    float growth;
    float scale = 1;
    float immunity = 0;
    int tickArgs[32];
    float timers[16]; // every other index stores actual timer? first stores default value?
    int state = 0;
    bool held;
    
    /// reserved is used by modes to hold dead object so its not overwritten?
    bool reserved = false;

    int energy; ///used for health loss, moving?
    bool flags[64];

    // array of pointers instead to use funcs? obj keeps count of references to it and then is freed
    Gobj *parent;
    //Gobj *target;

    Gobj *pointers[OBJ_POINTER_AMOUNT];
    int referencesTo = 0;

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

struct Inventory {
    int slots[8];
};
//void Inventory_Breakdown(Inventory *inv, int slot);
//void Inventory_Add(Inventory *inv, Gobj *obj, int amount = 1, int slot = -1);
//void Inventory_Add(Inventory *inv, int id, int amount = 1, int slot = -1);
//void Inventory_Remove(Inventory *inv, int slot);
//void Inventory_TransferToChild(Inventory *inv, int slot);
//void Inventory_AddChild(Inventory *inv, int slot);

struct ObjController {};

extern int totalObjects;
const int maxObjects = 1024;
extern Gobj objects[maxObjects];
extern GobjData objData[8];
extern Gobj_Func objFuncs[8];
extern Gobj *playerObj;
extern Gobj *o;
extern Gobj *selObj;

void Obj_Init();
void Obj_Tick();
void Obj_Render(Gobj *obj);
void Obj_Death(Gobj *obj);
void Obj_GiveHealth(Gobj *obj, int h);
void Obj_GetGlobalCenter(Gobj *obj, V2 *outPos);
void Obj_MoveTo(Gobj *objA, Gobj *objB, float f);
void Child_GiveBond(Gobj *obj, int child, int b);
/// checks if selObj needs updated, or for obj interactions
Gobj* Obj_CheckAtMouse();
#endif