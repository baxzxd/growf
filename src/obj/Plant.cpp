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

int Plant_Init() {
    //o->timers[0] = 2;
    //o->timers[1] = 1;
    Obj_SetFlag(o, STATIC, true);
    return 0;
}
int Plant_Use() {
    return 0;
}
int Plant_Update() {
    //draw darker shadow under plant to show health as animals eat it
    //water to grow
    // transparent green particles when bit
    bool tickGrowth = true;
    switch( o->state ) {
        // growing
        case 0:
            if( tickGrowth ) {
                o->timers[TIMERGROWTH] += del;
                if( o->timers[TIMERGROWTH] > 1 ) {
                    o->scale += 1.0f;
                    o->timers[TIMERGROWTH] = 0;
                }
            }
            else {

            }
            
            if( o->scale >= 10.0f ) {
                std::cout << "plant done grew" << std::endl;
                o->state = 1;
            }
        break;

        // decaying
        case 1:
            if( o->childCount == 5 )
                break;
            o->timers[0] -= del;
            if( o->timers[TIMERGROWTH] <= 0 ) {
                o->timers[TIMERGROWTH] = o->timers[TIMERGROWTH+1];
                //create fruit
                V2 supPos = {(float)(rand()%(int)(Obj_GetSize(o).x)),(float)(rand()%((int)Obj_GetSize(o).y))};
                Gobj* fruit = Obj_Create(3, o->pos,RandV2(100), 1);
                if( fruit ) {
                    fruit->energy = 10;
                    fruit->immunity = .6f;
                    // green to orange
                    //Obj_SetGrowGradient( Color_RGBToInt(0, 165, 0), Color_RGBToInt(255, 165, 0));
                    Obj_AddChild(o, fruit, 100, supPos);
                    Obj_SetFlag(fruit, LIVING, true);
                }
            }
        break;

        // dead
        case 2:

        break;
    }
    return 0;
}