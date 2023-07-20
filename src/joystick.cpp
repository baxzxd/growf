#include <iostream>
#include <algorithm>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"
#include "render.h"
#include "color.h"
#include "input.h"
#include "obj/ObjMain.h"
#include "obj/ObjData.h"
#include "obj/StandardObjs.h"
#include "audio.h"

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

float joystickAxes[6];


// same array just double length
int joyPressed[22];
int joyJustPressed[22];

V2 controllerCursorPos;
V2 controllerSmartCursorPos;
V2 joyV;
int triggerHeld[2];

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
}
SDL_Rect controllerCursorRect;
void Joystick_Update() {
    joyV = {joystickAxes[2],joystickAxes[3]};
    //controllerCursorPos = controllerCursorPos + joyV * 120.0f * del;
    controllerCursorPos = joyV * 120.0f;

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
}
void Joystick_Added(SDL_Event *e) {
    joystickIndex = e->cdevice.which;
    gameController = SDL_GameControllerOpen(joystickIndex);
    joystick = SDL_GameControllerGetJoystick(gameController);
        joystickID = SDL_JoystickInstanceID(joystick);
    // TODO
        
}
void Joystick_Removed(SDL_Event *e) {    
    joystickID = e->cdevice.which;
    // TODO
}
void Joystick_Button(SDL_Event *e) {
    joystickID = e->cbutton.which;
    button = (SDL_GameControllerButton)e->cbutton.button;
    std::cout<<button<<std::endl;
    isPressed = e->cbutton.state == SDL_PRESSED;
    joyPressed[button] = isPressed;
    if( isPressed )
        joyJustPressed[button] = 1;
    // TODO
}

void Joystick_AxisMotion(SDL_Event *e) {
    joystickID = e->caxis.which;

    axis = (SDL_GameControllerAxis)e->caxis.axis;
    value = (float)e->caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
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
}