#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "../main.h"
#include "../color.h"
#include "../render/render.h"
#include "ObjMain.h"
#include <vector>
#include "StandardObjs.h"

Gobj_Func objFuncs[9] = {
    {0,Player_Render,Player_Update,0,Player_Death},
    {Plant_Init,Plant_Render,Plant_Update, Plant_Use, Plant_Death},
    {0,0,Animal_Update,0,0},
    {0,0,Box_Update,0,0},
    {0,Hopper_Render,Hopper_Update,0,0},
    {0,0,0,Nail_Use,0},
    {0,0,Animal_Update,0,0},{0,0,Box_Update,0,0},
    {Weapon_Init,Weapon_Render,Weapon_Update,Weapon_Use,Weapon_Death}
};


// space has repeating borders
std::map<std::string, GobjData> objData;

void AddObj(std::string id, int health, V2Int size, Color color, Gobj_Func *type) {
   
    std::cout<<"Obj "<<id<<" added"<<std::endl;
    GobjData d;
    d.id = id;
    d.maxHealth = health;
    d.size = size;
    d.color = color;
    d.funcs = type;
    objData[id] = d;
}
void AddObjType() {} // obj funcs?


void Data_Init() {
    AddObj("player", 10, {16,16},Color{0,0,255},&objFuncs[0]);
    AddObj("tree",20, {8,8}, Color{0,200,25}, &objFuncs[1]);
    AddObj("dog",2, {10,10}, Color{232,122,0}, &objFuncs[2]);
    AddObj("box",50,{12,12}, Color{255,165,0}, &objFuncs[3]);
    AddObj("hopper",50,{16,16}, Color{200,0,255}, &objFuncs[4]);
    AddObj("log",50,{16,80}, Color{114,44,0}, &objFuncs[3]);
    AddObj("stone",50,{12,18}, Color{64,64,64}, &objFuncs[3]);
    AddObj("nail", 10, {6,14}, Color{128,128,128}, &objFuncs[5]);
    AddObj("ak47", 10, {2,20}, Color{164,180,70}, &objFuncs[8]);

    AddObj("apple", 10, {13,13}, Color{255,52,0}, &objFuncs[3]);
    AddObj("orange", 10, {10,10}, Color{255,165,0}, &objFuncs[3]);
    AddObj("corn", 10, {20,10}, Color{255,165,0}, &objFuncs[3]);
    objData["corn"].textures.push_back(Render_CreateSpeckledTexture(20,10,Color{255,255,64},Color{255,255,255}, 16,1,1));
}