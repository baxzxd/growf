#ifndef COLOR_H
#define COLOR_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"

typedef struct Color {
    int r;
    int g;
    int b;
    int a;
    int c;
};
uint32_t Color_rgb(double ratio);
int Color_GetColorValue();
Color Color_FromValues(int r, int g, int b, int a);
void Color_RGBFromInt(int c, int *r, int *g, int *b);
int Color_RGBToInt(int r, int g, int b);
int Color_Brightness(int c, float f);
int Color_LerpColor(int color, int finalColor, float fraction);
#endif