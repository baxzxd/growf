#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../main.h"
#include "../render/render.h"
#include "../color.h"
#include "window.h"
#include "../FastNoiseLite.h"
#include "../world/world.h"

void Button_Update() {
    if( SDL_PointInRect(&mousePoint, &win->rect) ) {
        win->state = 1;
        if( mouseClicked ) {
            win->state = 2;
            if( mouseJustClicked ) {
                if( win->voidFunc ) 
                    win->voidFunc();
            }
        }
    }
    else {
        win->state = 0;
    }
}
void Button_Render() {
    //CONVERT COLOR SHIT TO STRUCT DUMBASS
    switch(win->state) {
        case 0:
            win->c = Color_RGBToInt(255,0,0);
        break;

        case 1:
            win->c = Color_RGBToInt(0,255,0);

        break;

        default:
            win->c = Color_RGBToInt(0,0,255);

        break;
    }
}
Window* Button_Create(SDL_Rect r) {
    windows.resize(windows.size()+1);
    Window *w = &windows[windows.size()-1];
    w->pos = {r.x,r.y};
    w->rect = r;
    w->update = Button_Update;
    w->render = Button_Render;
    return w;
}