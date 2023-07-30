#ifndef OBJ_MAIN
#define OBJ_MAIN
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <map>
#include "../main.h"
#include "../color.h"

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
    std::string id;
    /// possibly rename to basescale
    V2Int size;
    // bitshift to get r,g,b?
    Color color;
    std::vector<SDL_Texture*> textures;
    int maxHealth = 100;
    Gobj_Func *funcs;
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
    STATIC = 8,
    OBJ_FLAG0 = 17,
    OBJ_FLAG1 = 18,
    OBJ_FLAG2 = 19,
    OBJ_FLAG3 = 20,
} Gobj_Flags;
struct Gobj;
struct Gobj_Child {
    Gobj* o;
    int bond;
    V2 pos;
    bool inactive = true;
    bool visible = true;
    bool giveEnergy;
    float energyTransfer;
};
const int OBJ_POINTER_AMOUNT = 8;
const int CHILD_COUNT = 8;
struct Gobj {
    std::string id;
    V2 pos;
    V2 cPos;
    V2 vel;
    V2 look;
    int rotation = 0;
    int team = 0;
    int subType;
    int health = 10;
    Color color;
    int growthStage;
    float growth;
    float scale = 1;
    V2 size;
    float immunity = 0;
    int tickArgs[32];
    float timers[16]; // every other index stores actual timer? first stores default value?
    int state = 0;
    bool held;
    
    /// reserved is used by modes to hold dead object so its not overwritten?
    bool reserved = false;

    int energy; ///used for health loss, moving?
    bool flags[64];
    int states[8];

    // array of pointers instead to use funcs? obj keeps count of references to it and then is freed
    //Gobj *parent;
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
extern std::map<std::string, GobjData> objData;
extern Gobj_Func objFuncs[9];
extern Gobj *playerObj;
extern Gobj *o;
extern Gobj *selObj;

void Obj_Init();
void Obj_Tick();
void Obj_Render(Gobj *obj);
void Obj_Death(Gobj *obj);
void Obj_GiveHealth(Gobj *obj, int h);
void Obj_MoveTo(Gobj *objA, Gobj *objB, float f);
void Child_GiveBond(Gobj *obj, int child, int b);
/// checks if selObj needs updated, or for obj interactions
Gobj* Obj_CheckAtMouse();
#endif