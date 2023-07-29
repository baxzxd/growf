/*
 * Copyright (c) 2018, 2019 Amine Ben Hassouna <amine.benhassouna@gmail.com>
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any
 * person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without
 * limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice
 * shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <iostream>
#include <algorithm>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"
#include "joystick.h"
#include "render/render.h"
#include "color.h"
#include "input.h"
#include "window/window.h"
#include "world/world.h"
#include "obj/ObjMain.h"
#include "obj/ObjData.h"
#include "obj/ObjUtil.h"
#include "obj/StandardObjs.h"
#include "audio.h"
#include "gamemode/Survival.h"
using namespace std;





// combine obj header files?

int keys[256];
int keysJustPressed[256];
int gameState = 0;
//Gobj* objGrid[13][128];
/// cellX,cellY are output
V2 RandV2(int r) {
    return {(float)((rand()%(r*2))-r),(float)((rand()%(r*2))-r)};
}
void GetObjCell(Gobj* obj, int *cellX, int *cellY) {
    // get furthest corners and divide by cell size
}
void Game_Reset() {
    
}
float del = 0;
Uint64 lastTick = 0;
Uint64 tick = 0;
V2 d, dNorm,mousePos;
float len;

bool usingJoystick = false;

int mouseX, mouseY;
V2 GetMousePos() {
    int mouseX,mouseY;
    SDL_GetMouseState(&mouseX,&mouseY);
    return {(float)mouseX, (float)mouseY};
}
V2 playerAim;
SDL_Rect mouseR;
void UpdateMouseRect() {
    mouseR.x = mousePos.x;
    mouseR.y = mousePos.y;
    mouseR.w = 4;
    mouseR.h = 4;
}

//remove direct access to joypressed and keypressed and add actions with bindings
bool mouseClicked = false;
bool mouseJustClicked = false;
V2Int tileMousePos;
V2 worldMousePos;
WorldTile *t;
int selectedTile = 0;int tilesChanged = 0;
void EnterGameLoop() {
    // Event loop exit flag
    bool quit = false;
    int speed = 1;
    
    // Event loop
    while(!quit) {
        SDL_Event e;

        // Wait indefinitely for the next available event
        SDL_PollEvent(&e);
        
        // calculate delta
        lastTick = tick;
        tick = SDL_GetPerformanceCounter();
        del = (float)((tick - lastTick)) / (float)(SDL_GetPerformanceFrequency());

        
        //


        //check window collision then world collision on click
        if( usingJoystick ) {
            mousePos = playerObj->pos + controllerCursorPos;
        }
        else {
            mousePos = GetMousePos();
        }
        worldMousePos = mousePos/cameraScale;// - cameraPos;
        std::cout<<worldMousePos.x/tileSize<<":"<<worldMousePos.y/tileSize<<std::endl;
        UpdateMouseRect();
        
        Window_Main();

        // Initialize renderer color white for the background
        Render_SetDrawColor(Color_RGBToInt(37,143,18),0xff);

        // Clear screen
        SDL_RenderClear(renderer);
        Render_SetDrawColor(Color_RGBToInt(255,0,0),0xff);
        World_Render();

        if( mouseJustClicked )
            mouseJustClicked = false;
            
        // ingame input?
        int chunkX, chunkY;
        switch(e.type) {
            // User requests quit
            case SDL_QUIT:
                quit = true;
            break;

            case SDL_MOUSEBUTTONDOWN:
                usingJoystick = false;

                switch(e.button.button){
                    case 1:
                        mouseClicked = true;
                        mouseJustClicked = true;
                        Player_Use();
                    break;
                    case 3:
                        Player_AltUse();
                    break;

                }
            break;
            case SDL_MOUSEBUTTONUP:
                    std::cout<<(int)e.button.button<<std::endl;
                    switch(e.button.button) {
                        case 1:
                        mouseClicked = false;
                        mouseJustClicked = false;
                        usingJoystick = false;
                        
                        Player_ReleaseUse();
                        break;
                    }
                break;

            case SDL_MOUSEWHEEL:
                Player_Scroll(e.wheel.y);
            break;
            
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                usingJoystick = false;

                if( e.key.keysym.sym == SDLK_ESCAPE ) {
                    Game_Reset();
                }
                ProcessKey(&e.key);
            break;

            case SDL_CONTROLLERDEVICEADDED:
                usingJoystick = true;
                Joystick_Added(&e);
            break;
            case SDL_CONTROLLERDEVICEREMOVED:
                usingJoystick = true;
                Joystick_Removed(&e);
            break;
            case SDL_CONTROLLERAXISMOTION:

                usingJoystick = true;
                Joystick_AxisMotion(&e);
                    
                
            break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                usingJoystick = true;
                Joystick_Button(&e);
            break;
	
        }
        

        // move actions outside of switch and use bools for action pressed




        SDL_RenderDrawRect(renderer, &controllerCursorRect);
        Obj_Tick();
        
        V2 pos = Obj_GetGlobalCenter(playerObj);
        //Render_String("393ffff93999",pos);
        // Update screen
        Render_Main();
        Window_Render();

        SDL_RenderPresent(renderer);

        memset(keysJustPressed, 0, sizeof(keysJustPressed));
        memset(joyJustPressed, 0, sizeof(joyJustPressed));

        Survival_Tick();
    }
}
int main(int argc, char* argv[])
{
    // Unused argc, argv
    (void) argc;
    (void) argv;
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Joystick_Init();

    //SDL_InitA
    initAudio();
    // init
    Render_Init();

    srand(time(NULL));
    
    Window_StaticInit();
    Data_Init();
    Obj_Init();
    World_Init();

    
    EnterGameLoop();

    // Destroy renderer
    SDL_DestroyRenderer(renderer);
    // Destroy window
    SDL_DestroyWindow(window);
    // Quit SDL
    SDL_Quit();

    return 0;
}
void GamePause(){}
