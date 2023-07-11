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
int grabSelection = 0;
int breakCount;
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
    
    V2 mouseRel = playerObj->cPos + (mousePos - playerObj->cPos).Norm() * 24;
    interactRect.x = (int)mouseRel.x - interactRect.w/2;
    interactRect.y = (int)mouseRel.y - interactRect.h/2;

    Render_SetDrawColor(Color_RGBToInt(0,125,200),255);
    SDL_RenderDrawRect(renderer,&interactRect);
    
    /*
    grabbing
    tap to grab
    hold to choose and use?
    double tap to drop?
    */
    if( keys[SDL_SCANCODE_G] ) {
        //try grba
        if( keysJustPressed[SDL_SCANCODE_G] ) {
            CollInfo coll;
            Obj_GetOverlaps(&interactRect, &coll);
            for( int i = 0; i < coll.overlap; i++ ) {
                if( coll.overlaps[i] == playerObj || Obj_HasFlag(coll.overlaps[i], STATIC) )
                    continue;
                Obj_AddChild(playerObj, coll.overlaps[i], 100, coll.overlaps[i]->pos - playerObj->pos, true);
            }
            grabSelection = 0;
        }
    }
    if( keysJustPressed[SDL_SCANCODE_T] ) {
        for( int i = 0; i < CHILD_COUNT; i++ ) {
            if( playerObj->children[i].inactive )
                continue;
            
            Gobj_Child ch = playerObj->children[i];
            Child_GiveBond(playerObj, i, -ch.bond);
            ch.o->vel = playerAim * 800;
            ch.o->energy = -50;
            ch.o->immunity = 0;
        }
    }
    int grabChange = keysJustPressed[SDL_SCANCODE_RIGHT] - keysJustPressed[SDL_SCANCODE_LEFT];
    
    if( grabChange ) {

    //default to -1 each grab? 
        //while loop for dumb array gaps
        breakCount = -1;
        while(true) {
            breakCount += 1;
            if( breakCount == CHILD_COUNT )
                break;
            grabSelection = (grabSelection + grabChange)%CHILD_COUNT;
            if( grabSelection < 0 )
                grabSelection = CHILD_COUNT - 1;
            if( playerObj->children[grabSelection].inactive)
                continue;
            selObj = playerObj->children[grabSelection].o;
            break;
        }
    }
    
    
    if( grabSelection != -1 && !playerObj->children[grabSelection].inactive ) {
        Gobj_Child *ch = &playerObj->children[grabSelection];
        ch->pos = playerAim * 30.0f;
    }
    
    lastPlayerDir = playerDir;
    return 0;
}

void Player_Use() {
    selObj = Obj_CheckAtMouse();
    if( grabSelection != -1 && !playerObj->children[grabSelection].inactive ) {
        Gobj_Child ch = playerObj->children[grabSelection];
        
        // PASS POINTER TO FUNC? DAMN
        if( ch.o->data->funcs->funcUse ) {
            ch.o->data->funcs->funcUse(ch.o);

            if( Obj_HasFlag(ch.o, STATIC) )
                Obj_RemoveChild(playerObj, grabSelection);
        }
    }
}
void Player_ReleaseUse() {
    Gobj *hovered = Obj_CheckAtMouse();

    if( !selObj || !hovered )
        return;
    
    //selObj->target = hovered;
    std::cout<<"outher sel"<<std::endl;
}