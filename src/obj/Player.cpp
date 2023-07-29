#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../joystick.h"
#include "../color.h"
#include "../render/render.h"
#include "../world/world.h"

#include "ObjMain.h"
#include "ObjUtil.h"
#include "ObjPhys.h"
#include "StandardObjs.h"

#include "../audio.h"
std::vector<InventorySlot> inventory;
int selSlot = 0;
// create slots in world when changed?
//flipping tile when broken?
void Inventory_AddTiles(InventorySlot *slot) {
    int e = -1;
    for( int s = 0; s < inventory.size(); s++ ) {
        if( slot->id == inventory[s].id ) {
            e = s;
            break;
        }
    }
    std::cout<<"player got "<<slot->amount<<" "<<slot->id<<std::endl;

    if( e != -1 ) {
        inventory[e].amount += slot->amount;
    }
    else {
        inventory.resize(inventory.size()+1);
        inventory[inventory.size()-1].id = slot->id;
        inventory[inventory.size()-1].amount = slot->amount;
    }
}
void Player_AddWorldChange() {
    for( int i = 0; i < change.tileTypes; i++ ) {
        Inventory_AddTiles(&change.slots[i]);
    }
}

V2 playerDir;
SDL_Rect interactRect = {0,0,32,32};
SDL_Rect interactRectVisual = {0,0,32,32};

V2 lastPlayerDir;
int controls[32];
int grabSelection = 0;
int breakCount;
// grab all objects in interact rect and cycle through them
// only allow certain number of objects to allow throwing as projectiles
// fuse everything together?
//default to -1 each grab? 
    //while loop for dumb array gaps


//tile icon over mouse that gets lerped into position and once in place regrows on mouse
//dial for tile inventory?
bool tileLerpActive = false;
float tileLerpTime = .2f;
float tileLerp = tileLerpTime;
V2 tilePos;
V2 clickPos;
V2 tileLerpDiff;
SDL_Rect tileRect = { 0, 0, tileSize, tileSize };

void OnTilePlaced(V2 p) { //[CHANGE] maybe pass tile
}
int Player_Death(Gobj *obj) {
    Game_Reset();
    return 0;
}

//press c to start combine with red outline and action or something to cancel?
//generalize this?
void Player_ChangeGrabbed( int change ) {
    if( !change )
        return;
    breakCount = -1;
    while(true) {
        breakCount += 1;
        if( breakCount == CHILD_COUNT )
            break;
        grabSelection = (grabSelection + change)%CHILD_COUNT;
        if( grabSelection < 0 )
            grabSelection = CHILD_COUNT - 1;
        if( playerObj->children[grabSelection].inactive)
            continue;
        //selObj = playerObj->children[grabSelection].o;
        break;
    }
}
bool Player_HasGrab() {
    return grabSelection != -1 && !playerObj->children[grabSelection].inactive;
}
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

    // fire projectile
    V2 d = worldMousePos - playerObj->pos;
    o->look = d.Norm();

    V2 playerWorld = Obj_GetGlobalCenter(playerObj);
    V2 mouseRel = playerWorld + (((mousePos) - playerWorld).Norm() * 24);

    V2 playerCam = Obj_GetCameraCenter(playerObj);
    V2 mouseRelCam = playerCam + (((mousePos+cameraPos) - playerCam).Norm() * 24);
    interactRect.x = (int)mouseRel.x - interactRect.w/2;
    interactRect.y = (int)mouseRel.y - interactRect.h/2;
    interactRectVisual.x = (int)mouseRelCam.x - interactRect.w/2;
    interactRectVisual.y = (int)mouseRelCam.y - interactRect.h/2;

    Render_SetDrawColor(Color_RGBToInt(0,125,200),255);
    SDL_RenderDrawRect(renderer,&interactRectVisual);
    
    /*
    grabbing
    tap to grab
    hold to choose and use?
    double tap to drop?
    */
    if( keys[SDL_SCANCODE_G] || joyPressed[SDL_CONTROLLER_BUTTON_B] ) {
        //try grba
        if( keysJustPressed[SDL_SCANCODE_G] || joyJustPressed[SDL_CONTROLLER_BUTTON_B]) {
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
    if( keysJustPressed[SDL_SCANCODE_R] ) {
        if( Player_HasGrab() ) {
            Obj_Rotate(playerObj->children[grabSelection].o, 90);
        }
    }
    bool hardThrow = false;
    if( keysJustPressed[SDL_SCANCODE_T] ) {
        if( hardThrow ) {
            for( int i = 0; i < CHILD_COUNT; i++ ) {
                if( playerObj->children[i].inactive )
                    continue;

                Obj_ThrowChild( playerObj, i, playerAim );
                
            }
        } 
        else {
            Obj_ThrowChild( playerObj, grabSelection, playerAim );
        }
    }
    int grabChange = (keysJustPressed[SDL_SCANCODE_RIGHT] - keysJustPressed[SDL_SCANCODE_LEFT]) +
                     (joyJustPressed[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER]-joyJustPressed[SDL_CONTROLLER_BUTTON_LEFTSHOULDER]);
    Player_ChangeGrabbed(grabChange);
    
    if( Player_HasGrab() ) {
        Gobj_Child *ch = &playerObj->children[grabSelection];
        ch->pos = playerAim * 30.0f;
    }
    
    lastPlayerDir = playerDir;
    Render_SetCameraOffset(o->pos);
    return 0;
}
void Player_Use() {
    //selObj = Obj_CheckAtMouse();
    if( grabSelection != -1 && !playerObj->children[grabSelection].inactive ) {
        Gobj_Child ch = playerObj->children[grabSelection];
        
        // PASS POINTER TO FUNC? DAMN
        if( ch.o->data->funcs->funcUse ) {
            ch.o->look = o->look;
            ch.o->data->funcs->funcUse(ch.o);

            if( Obj_HasFlag(ch.o, STATIC) )
                Obj_RemoveChild(playerObj, grabSelection);
        }
    }
    else {
        if( !inventory.size() ) 
            return;
        int c = World_ChangeTile(inventory[selSlot].id, V2Int{(int)worldMousePos.x,(int)worldMousePos.y}, 0);
        if( c ) {
            playSound("grass.wav", 64);

            inventory[selSlot].amount -= 1;
            if( !inventory[selSlot].amount )
                inventory.erase(inventory.begin()+selSlot);
            if( tileLerpActive ) {
                tilePos = worldMousePos;
                clickPos = mousePos;
                tileLerpDiff = tilePos - V2{(float)tileRect.x, (float)tileRect.y};
                tileLerp = 0;
            }
        }
    }
}
void Player_AltUse() {
    // remove tiles at mouse
    int c = World_ChangeTile({}, V2Int{(int)worldMousePos.x,(int)worldMousePos.y}, 0);
    if( c ) {
        playSound("grass.wav", 64);
        Player_AddWorldChange();
    }
}
void Player_ReleaseUse() {
    
    /* 
    } */
    //if( !selObj || !hovered )
    //    return;
    
    //selObj->target = hovered;
}
void Player_Scroll(int dir) {
    cameraScale += .1f * dir;
    if( !inventory.size() )
        return;
    selSlot = (selSlot + dir) % inventory.size();
    if( selSlot < 0 )
        selSlot = inventory.size() - 1;
    std::cout<<selSlot<<std::endl;
}

V2Int inventoryPadding = {8,8};
V2Int inventorySize = {64,64};
int inventoryFontSize = 24;
// highlight similar tiles under mouse if button is held?
void Player_RenderInventory() {
    if( !inventory.size() )
        return;
    
    Render_SetDrawColor(Color_RGBToInt(200,200,200), 64);
    tileRect.x = 28;
    tileRect.y = SCREEN_HEIGHT - 112;
    tileRect.w = inventory.size() * (inventorySize.x + inventoryPadding.x) - inventoryPadding.x + 8; //remove extra padding?
    tileRect.h = inventorySize.y + inventoryFontSize + inventoryPadding.y;
    SDL_RenderDrawRect(renderer, &tileRect);



    //add render rect functions to render to handle camera scaling and position
    //just pass v2int and ints to function instead















    // render hotbar
    Render_String("INV", {32, SCREEN_HEIGHT - 80},16,24);
    for( int i = 0; i < inventory.size(); i++ ) {
        tileRect.x = 32 + (inventorySize.x + inventoryPadding.x) * i ;
        tileRect.y = SCREEN_HEIGHT - inventorySize.y - inventoryPadding.y*2;
        tileRect.w = inventorySize.x;
        tileRect.h = inventorySize.y;
        SDL_RenderCopy(renderer, tileData[inventory[i].id].surfaces[0], NULL, &tileRect);

        Render_String(std::to_string(inventory[i].amount),{(float)(32 + 32 * i), (float)(SCREEN_HEIGHT-32)}, 16,16);
    }

    // render tile at mouse
    int tSize = tileSize * 1.4f;
    int tileRX = (mousePos.x + tileSize), tileRY = (mousePos.y + tileSize);
    if( tileLerp < tileLerpTime ) {
        float f = tileLerp / tileLerpTime;
        tileRX = (int)((clickPos.x + tileLerpDiff.x*f));
        tileRY = (int)((clickPos.y + tileLerpDiff.y*f));
        tileLerp += del;
    }
    tileRect.x = tileRX - 3;
    tileRect.y = tileRY - 3;
    tileRect.w = tSize + 6;
    tileRect.h = tSize + 6;

    //[SET COLOR FOR DISTANCE CHECK]
    Render_SetDrawColor(Color_RGBToInt(255,255,255),255);
    SDL_RenderDrawRect(renderer, &tileRect);

    // position far enough away from mouse for animation but move depending if close to window borders
    tileRect.x = tileRX;
    tileRect.y = tileRY;
    tileRect.w = tSize;
    tileRect.h = tSize;
    SDL_RenderCopy(renderer, tileData[inventory[selSlot].id].surfaces[0], NULL, &tileRect);
}
int Player_Render() {
    Player_RenderInventory();
    
    return 0;
}