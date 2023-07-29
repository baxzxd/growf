#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include "main.h"
#include "color.h"

void SetRenderDrawColor(int c) {
}

int Color_LerpColor(int color, int finalColor, float fraction) {
    int r,g,b,fR,fG,fB;
    Color_RGBFromInt(color, &r, &g, &b);
    Color_RGBFromInt(finalColor, &fR, &fG, &fB);

    return (int) ((fR - r) * fraction + r) << 16 |
                (int) ((fG - g) * fraction + g) << 8 |
                (int) ((fB - b) * fraction + b);
}
//input: ratio is between 0.0 to 1.0
//output: rgb color
int Color_Brightness(int color, float f) {
    int r,g,b;
    Color_RGBFromInt(color, &r,&g,&b);

    r *= f;
    g *= f;
    b *= f;
    return Color_RGBToInt(r,g,b);
}
Color Color_FromValues(int r, int g, int b, int a) {
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    c.c = Color_RGBToInt(r,g,b);
    return c;
}

uint32_t Color_rgb(double ratio)
{
    //we want to normalize ratio so that it fits in to 6 regions
    //where each region is 256 units long
    int normalized = (int)(ratio * 256 * 6);

    //find the region for this position
    int region = normalized / 256;

    //find the distance to the start of the closest region
    int x = normalized % 256;

    uint8_t r = 0, g = 0, b = 0;
    switch (region)
    {
    case 0: r = 255; g = 0;   b = 0;   g += x; break;
    case 1: r = 255; g = 255; b = 0;   r -= x; break;
    case 2: r = 0;   g = 255; b = 0;   b += x; break;
    case 3: r = 0;   g = 255; b = 255; g -= x; break;
    case 4: r = 0;   g = 0;   b = 255; r += x; break;
    case 5: r = 255; g = 0;   b = 255; b -= x; break;
    }
    return r + (g << 8) + (b << 16);
}
int Color_GetColorValue(int c, int v) {
    return 0;
}
void Color_RGBFromInt(int c, int *outR, int *outG, int *outB) {
    int r,g,b;
    b = c % 256;
    g = ((c-b)/256) % 256;
    r = ((c-b)/(256*256)) - g/256;

    *outR = r;
    *outG = g;
    *outB = b;
}
int Color_RGBToInt(int r, int g, int b) {
    return (256*256)*r+256*g+b;
}