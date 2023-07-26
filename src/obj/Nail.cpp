//if not static render as line
#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render/render.h"
#include "ObjMain.h"
#include "ObjUtil.h"
#include "ObjPhys.h"
#include "StandardObjs.h"

int Nail_Use(Gobj *obj) {
    //check for overlaps at use position
    //if its not living nail it down with bond and damage nail 
    //      child flag to keep object static for give flag?

    CollInfo c;
    Obj_GetOverlaps(&mouseR, &c);
        std::cout<<"nailloverlap"<<c.overlap<<std::endl;

    for( int i = 0; i < c.overlap; i++ ) {
        if( Obj_HasFlag(c.overlaps[i], LIVING) )
            continue;
        //try to add child and break if added
        if( Obj_AddChild(c.overlaps[i], obj, 500, mousePos - c.overlaps[i]->pos, true) ) {
            Obj_SetFlag(c.overlaps[i], STATIC, true);
            break;
        }
    }
    return 0;
}