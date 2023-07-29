#ifndef STANDARDOBJS_H
#define STANDARDOBJS_H
#include "Plant.h"
typedef enum {
    C_GRAB = 0,
    C_USE = 1,
    C_JUMP = 2,
    C_ACTION = 3,
    C_ACTION2 = 4,
    C_ACTION3 = 5,
    C_ALTACTION = 6
} CONTROLS;
extern int controls[32];

struct Animal_Family {
    Gobj *leader;
    Gobj *members[16];
};
struct Animal_Data : GobjData {

};
void Animal_Init();
void Player_Init();
void Box_Init();
int Animal_Update();
int Player_Update();
void Player_Use();
void Player_AltUse();
void Player_ReleaseUse();
int Player_Death(Gobj *obj);
int Box_Update();
int Hopper_Update();

void Player_Scroll(int dir);
int Weapon_Use(Gobj *obj);
int Nail_Use(Gobj *obj);

int Hopper_Render();
int Player_Render();
#endif