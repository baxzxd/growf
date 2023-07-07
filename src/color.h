#ifndef COLOR_H
#define COLOR_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"

typedef enum {
    r,
    g,
    b
} ENUMCOLOR;

uint32_t Color_rgb();
int Color_GetColorValue();
void Color_RGBFromInt(int c, int *r, int *g, int *b);
int Color_RGBToInt(int r, int g, int b);
int Color_Brightness(int c, float f);
int Color_LerpColor(int color, int finalColor, float fraction);
#endif