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


void Slider_Init() {
}
void Slider_Render() {
    Render_SetDrawColor(Color_RGBToInt(128,128,128),128);
    SDL_RenderFillRect( renderer, &win->rects[0] );
}
void Slider_Update() {
    // sliderdragging probal
    if( win->dragging) {
        win->rect.x = mousePos.x;
    }
    else if( win->stoppedDragging ) {
        //signal
        float sliderV = (float)(win->rect.x - win->rects[0].x) / (float)win->rects[0].w;
        if( win->floatFunc ) {
            win->floatFunc(sliderV);
        }
    }
}


Window *Slider_Create(SDL_Rect handle, SDL_Rect bar) {
    windows.resize(windows.size()+1);

    Window *w = &windows[windows.size()-1];
    w->pos = {handle.x,handle.y};
    w->rect = {handle.x,handle.y,16,12};
    w->rects.resize(1);
    w->rects[0] = bar;
    w->funcInit = Slider_Init;
    w->update = Slider_Update;
    w->render = Slider_Render;
    return w;
}