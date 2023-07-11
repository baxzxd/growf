#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render.h"
#include "ObjMain.h"
#include "ObjPhys.h"

#include "StandardObjs.h"

Gobj_Func objFuncs[8] = {
    {0,0,Player_Update,0,0},{Plant_Init,Plant_Render,Plant_Update, Plant_Use, Plant_Death},{0,0,Animal_Update,0,0},
    {0,0,Box_Update,0,0},
    {0,Hopper_Render,Hopper_Update,0,0},{0,0,Plant_Update,0,0},
    {0,0,Animal_Update,0,0},{0,0,Box_Update,0,0}
};


// space has repeating borders
GobjData objData[6] {
    {"player", 10, {16,16},255,&objFuncs[0]},
    {"Tree",20, {8,8}, Color_RGBToInt(0,200,25), &objFuncs[1]},
    {"Dog",2, {10,10}, Color_RGBToInt(232,122,0), &objFuncs[2]},
    {"Box",50,{12,12}, Color_RGBToInt(255,165,0), &objFuncs[3]},
    {"Hopper",50,{16,16}, Color_RGBToInt(200,0,255), &objFuncs[4]},
    {"Log",50,{16,80}, Color_RGBToInt(114,44,0), &objFuncs[3]}
};