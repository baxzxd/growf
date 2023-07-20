#ifndef OBJDATA_H
#define OBJDATA_H
#include <string>

#include "../main.h"
#include "../color.h"
#include "../render.h"

#include "ObjMain.h"
void Data_Init();
void AddObj(std::string id, int health, V2Int size, int color, Gobj_Func *type);
#endif