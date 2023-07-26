#ifndef WINDOW_H
#define WINDOW_H
#include <SDL2/SDL.h>
#include <vector>
struct Window {
    bool init = false;
    V2Int pos;
    int state;
    int c = Color_RGBToInt(200,0,200);
    bool dragging;
    bool stoppedDragging;
    SDL_Rect rect;
    std::vector<SDL_Rect> rects;
    void (*funcInit)();
    void (*update)();
    //prerender
    void (*render)();
    void (*floatFunc)(float f);
    void (*voidFunc)();
};
// base window rect is slider
struct UISlider {
    SDL_Rect bar;
};

extern std::vector<Window> windows;
extern Window *win;
extern SDL_Point mousePoint;
Window* Button_Create(SDL_Rect r);
Window *Slider_Create(SDL_Rect handle, SDL_Rect bar);
void Window_StaticInit();
void Window_Main();
void Window_Render();
#endif