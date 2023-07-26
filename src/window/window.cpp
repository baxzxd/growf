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

SDL_Texture *noiseee;
SDL_Point mousePoint;
std::vector<Window> windows;
Window *win;

//tile icon over mouse that gets lerped into position and once in place regrows on mouse
//dial for tile inventory?
float tileLerp = 0;
float tileLerpTime = .2f;
V2Int tilePos;
V2Int clickPos;
void OnTilePlaced(V2Int p) { //[CHANGE] maybe pass tile
    tilePos = p;
    clickPos = {(int)mousePos.x,(int)mousePos.y};
    tileLerp = 0;
}
float f1;
float f2;
void ValueChange1(float f) {
    f1 = f;
    noiseee = Noise_GenerateSimplex(128, 1, f1,f2);

}
void ValueChange2(float f) {
    f2 = f;
    noiseee = Noise_GenerateSimplex(128, 1, f1,f2);

}
void ButtonClick() {
    std::cout<<"BUTTON CLICK"<<std::endl;
    save_texture(renderer,noiseee, "testNoise.png");
}
void Window_StaticInit() {
    Window *s = Slider_Create({128,128,16,12},{128,128,128,8});
    s->floatFunc = ValueChange1;
    Window *s2 = Slider_Create({128,160,16,12},{128,160,128,8});
    s2->floatFunc = ValueChange2;
    Window *b1 = Button_Create({16,16,32,32});
    b1->voidFunc = ButtonClick;
}
void Window_Main() {
    mousePoint.x = mousePos.x;
    mousePoint.y = mousePos.y;

    //if( tileLerp < tileLerpTime ) {
    //    tilePreview lerp
    //}

    //dither over layer1 tiles for height?
    for( int i = 0; i < windows.size(); i++ ) {
        win = &windows[i];
        if( !win->init  ) {
            if(win->funcInit)
                win->funcInit();
            win->init = true;
        }

        
        if( win->update )
            win->update();

        windows[i].stoppedDragging = false;
        if( !windows[i].dragging ) {
            if( SDL_PointInRect(&mousePoint, &windows[i].rect) && mouseClicked ) {
                windows[i].dragging = true;
            }
        }
        else {
            if( !mouseClicked ) {
                windows[i].dragging = false;
                windows[i].stoppedDragging = true;
            }
        }
    }

}
void Window_Render() {
    Render_String("WINDOWWW", {128,128},12,15);
    textRect.x = 0;
    textRect.y = 0;
    textRect.w = 128;
    textRect.h = 128;
    SDL_RenderCopy(renderer, noiseee, NULL, &textRect);


    for( int i = 0; i < windows.size(); i++ ) {
        win = &windows[i];
        if( windows[i].render ) {
            windows[i].render();
        }
        Render_SetDrawColor(win->c,128);
        SDL_RenderFillRect( renderer, &windows[i].rect );
    }
}