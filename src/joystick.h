#ifndef MAIN_JOYSTICK_H
#define MAIN_JOYSTICK_H

#include "main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


// joystick
extern CONTROLLERSTICK stick;
extern SDL_JoystickID joystickID;
extern SDL_GameControllerButton button;
extern SDL_GameControllerAxis axis;
extern SDL_GameController* gameController;
extern SDL_Joystick* joystick;
extern float value;
extern bool isPressed;
extern SDL_Rect controllerCursorRect;
extern int joystickIndex;

extern int joyPressed[22];
extern int joyJustPressed[22];
extern float joystickAxes[6];

extern float joystickAxes[6];
extern int joyPressed[22];
extern int joyJustPressed[22];
extern V2 controllerCursorPos;
extern V2 controllerSmartCursorPos;
extern V2 joyV;
extern int triggerHeld[2];

void Joystick_Init();
void Joystick_Added(SDL_Event *e);
void Joystick_Removed(SDL_Event *e);
void Joystick_AxisMotion(SDL_Event *e);
void Joystick_Button(SDL_Event *e);
#endif