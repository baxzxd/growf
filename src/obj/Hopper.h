#ifndef HOPPER_H
#define HOPPER_H

#include "ObjMain.h"

int Hopper_Init();
int Hopper_Update();
int Hopper_Death(Gobj *obj);
int Hopper_Use(Gobj *obj);

typedef enum HOPPER_TYPE {
    HOPPER_SORT,
    HOPPER_SUCK,
    HOPPER_BLOW
};
void Hopper_Sort();
void Hopper_SuckItem();
void Hopper_BlowItem();
#endif