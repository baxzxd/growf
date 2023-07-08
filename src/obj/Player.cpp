#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../color.h"
#include "../render.h"
#include "ObjMain.h"
#include "ObjUtil.h"
#include "ObjPhys.h"
#include "StandardObjs.h"

V2 playerDir;
SDL_Rect interactRect = {0,0,32,32};
V2 lastPlayerDir;
int controls[32];

// grab all objects in interact rect and cycle through them
// only allow certain number of objects to allow throwing as projectiles
// fuse everything together?
int Player_Update() {
    V2 joyAxes = {joystickAxes[0], joystickAxes[1] };
    if( abs(joyAxes.x) < .05f )
        joyAxes.x = 0;
    if( abs(joyAxes.y) < .05f )
        joyAxes.y = 0;
    V2 keyAxes = {(float)(keys[7]-keys[4]), (float)(keys[22]-keys[26])}; 
    playerDir = (joyAxes + keyAxes).Norm();
    
    float vel = 240;
    o->vel = o->vel + V2{(float)playerDir.x,(float)playerDir.y} * (vel * del); 
    
    interactRect.x = (int)(playerObj->cPos.x + playerDir.x* 24) - interactRect.w/2;
    interactRect.y = (int)(playerObj->cPos.y + playerDir.y* 24) - interactRect.h/2;

    Render_SetDrawColor(Color_RGBToInt(0,125,200),255);
    SDL_RenderDrawRect(renderer,&interactRect);
    
    if( keys[SDLK_g] ) {
        CollInfo coll;
        Obj_GetOverlaps(&interactRect, &coll);
        for( int i = 0; i < coll.overlap; i++ ) {
            if( coll.overlaps[i] == playerObj )
                continue;
            Obj_AddChild(playerObj, coll.overlaps[i], 100, coll.overlaps[i]->pos - playerObj->pos);
        }
    }
    
    lastPlayerDir = playerDir;
    return 0;
}

Gobj* proj;
void Player_Use() {
    selObj = Obj_CheckAtMouse();
    // fire projectile
    d = mousePos - playerObj->pos;
    dNorm = d.Norm();
    proj = Obj_Create(3,playerObj->pos + dNorm * 24.0f, dNorm * 500.0f, .5f);
    proj->energy = -50.0f;
}
void Player_ReleaseUse() {
    Gobj *hovered = Obj_CheckAtMouse();

    if( !selObj || !hovered )
        return;
    
    selObj->target = hovered;
    std::cout<<"outher sel"<<std::endl;
}