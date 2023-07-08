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
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"
#include "render.h"
#include "color.h"
#include "input.h"
#include "obj/ObjMain.h"
#include "obj/StandardObjs.h"
#include "audio.h"
using namespace std;


int keys[256];
int gameState = 0;
//Gobj* objGrid[13][128];
/// cellX,cellY are output
V2 RandV2(int r) {
    return {(float)((rand()%(r*2))-r),(float)((rand()%(r*2))-r)};
}
void Joystick_Init() {
    // set flags n such
    SDL_SetHint(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS, "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    SDL_Init(SDL_INIT_GAMECONTROLLER);
    SDL_SetHint(SDL_HINT_JOYSTICK_THREAD, "1");

    // find controllers
    int numGamepads = 0;
    SDL_GameController* gamepadId[12] = { nullptr };
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (!SDL_IsGameController(i)) {
            continue;
        }
        SDL_GameController* g = SDL_GameControllerOpen(i);
        gamepadId[numGamepads++] = g;
    }
    std::cout<<"FOUND "<< sizeof(Animal_Data) << "GAMEPADS";
}
void GetObjCell(Gobj* obj, int *cellX, int *cellY) {
    // get furthest corners and divide by cell size
}
void Game_RegenWorld() {
    
}
float del = 0;
Uint64 lastTick = 0;
Uint64 tick = 0;
V2 d, dNorm,mousePos;
float len;
float joystickAxes[6];
V2 controllerCursorPos;
V2 controllerSmartCursorPos;
V2 joyV;
int triggerHeld[2];
bool usingJoystick = false;
int mouseX, mouseY;
V2 GetMousePos() {
    int mouseX,mouseY;
    SDL_GetMouseState(&mouseX,&mouseY);
    return {(float)mouseX, (float)mouseY};
}

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

        // joystick
        CONTROLLERSTICK stick;
        SDL_JoystickID joystickID;
        SDL_GameControllerButton button;
        SDL_GameControllerAxis axis;
        SDL_GameController* gameController;
        SDL_Joystick* joystick;
        float value;
        bool isPressed = false;
        int joystickIndex = 0;
        
        if( usingJoystick ) {
            mousePos = playerObj->pos + controllerCursorPos;
        }
        else {
            mousePos = GetMousePos();
        }

    
        switch(e.type) {
            // User requests quit
            case SDL_QUIT:
                quit = true;
            break;

            case SDL_MOUSEBUTTONDOWN:
                usingJoystick = false;

                switch(e.button.button){
                    case 1:
                        Player_Use();
                    break;
                    case 2:
                        Player_ReleaseUse();
                    break;

                }
            break;
            case SDL_MOUSEBUTTONUP:
                usingJoystick = false;
                Player_ReleaseUse();
            break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                usingJoystick = false;

                if( e.key.keysym.sym == SDLK_ESCAPE ) {
                    Game_RegenWorld();
                }
                ProcessKey(&e.key);
            break;

            case SDL_CONTROLLERDEVICEADDED:
                usingJoystick = true;
            
                joystickIndex = e.cdevice.which;
                gameController = SDL_GameControllerOpen(joystickIndex);
                joystick = SDL_GameControllerGetJoystick(gameController);
                 joystickID = SDL_JoystickInstanceID(joystick);
                // TODO
            
            break;
            case SDL_CONTROLLERDEVICEREMOVED:
                usingJoystick = true;
            
                joystickID = e.cdevice.which;
                // TODO
            
            break;
            case SDL_CONTROLLERAXISMOTION:
                usingJoystick = true;
            
                joystickID = e.caxis.which;
                
                axis = (SDL_GameControllerAxis)e.caxis.axis;
                value = (float)e.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
                joystickAxes[axis] = value;
                switch( axis ) {
                    case 0:
                    case 1:
                        stick = LEFTSTICK;
                    break;

                    case 2:
                    case 3:
                        stick = RIGHTSTICK;
                    break;
                }

                //cursor modes, terraria console style but trigger toggles "smart cursor" for aiming
                //deadzones here?
            break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                usingJoystick = true;
            
                joystickID = e.cbutton.which;
                button = (SDL_GameControllerButton)e.cbutton.button;
                isPressed = e.cbutton.state == SDL_PRESSED;
                // TODO
            
            break;
	
        }
        

        // move actions outside of switch and use bools for action pressed



        joyV = {joystickAxes[2],joystickAxes[3]};
        //controllerCursorPos = controllerCursorPos + joyV * 120.0f * del;
        controllerCursorPos = joyV * 120.0f;

        SDL_Rect controllerCursorRect;
        controllerCursorRect.x = (int)playerObj->pos.x + (int)controllerCursorPos.x - 8;
        controllerCursorRect.y = (int)playerObj->pos.y + (int)controllerCursorPos.y - 8;
        controllerCursorRect.w = 16;
        controllerCursorRect.h = 16;
        
        // one click per joystick pull since its analog
        if( triggerHeld[0] ) {
            if( joystickAxes[4] < .5f )
                triggerHeld[0] = 0;
        }
        else if( joystickAxes[4] > .5f ) {
            triggerHeld[0] = 1;
            Player_Use();
        }

        // Initialize renderer color white for the background
        Render_SetDrawColor(Color_RGBToInt(0x13,0,0),0xff);

        // Clear screen
        SDL_RenderClear(renderer);
        Render_SetDrawColor(Color_RGBToInt(255,0,0),0xff);

        SDL_RenderDrawRect(renderer, &controllerCursorRect);
        Obj_Tick();
        
        V2 pos;
        Obj_GetGlobalCenter(playerObj, &pos);
        //Render_String("393ffff93999",pos);
        // Update screen
        SDL_RenderPresent(renderer);
    }
}
int main(int argc, char* argv[])
{
    // Unused argc, argv
    (void) argc;
    (void) argv;
    
    TTF_Init();
    Joystick_Init();

    // init
    Render_Init();
    Obj_Init();

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
