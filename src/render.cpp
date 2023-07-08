#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "color.h"
#include "audio.h"
#include "render.h"

using namespace std;

// rendering
SDL_Window *window;
SDL_Renderer *renderer;

// graphics
SDL_Rect squareRect;
TTF_Font* font;

SDL_Rect textRect = {120,120,24,30};

void Render_String(string s, V2 pos, int w, int h) {
    textRect.w = w;
    textRect.h = h;
    for( int i = 0; i < s.length(); i++ ) {
        textRect.x = pos.x + textRect.w * i;
        textRect.y = pos.y - (textRect.h + 4);
        SDL_RenderCopy(renderer,charTextures[(int)s[i]], NULL, &textRect);
    }
}

int Render_Init() {
    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
        //std::cout << "SDL could not be initialized!" << std::endl
        //          << "SDL_Error: " << SDL_GetError() << std::endl;
        return 0;
    }
    //initAudio();

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        //std::cout << "SDL can not disable compositor bypass!" << std::endl;
        return 0;
    }
#endif


    // Create window
    window = SDL_CreateWindow(WINDOW_TITLE,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if(!window)
    {
        //std::cout << "Window could not be created!" << std::endl
        //          << "SDL_Error: " << SDL_GetError() << std::endl;
        return 0;
    }


    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        //std::cout << "Renderer could not be created!" << std::endl
        //            << "SDL_Error: " << SDL_GetError() << std::endl;
        return 0;
    }

    Render_CharToTextures(48,255,255,255,0,0,0);

    return 1;

}

SDL_Texture *Render_PreRenderString(char* s) {
    return 0;
}
SDL_Texture *Render_PreRenderStringsFromFile(char* path) {
    return 0;
}
void Render_SetDrawColor(int c, int alpha) {
    int r,g,b;
    Color_RGBFromInt(c,&r,&g,&b);
    SDL_SetRenderDrawColor(renderer, r,g,b,alpha);
}

SDL_Texture *charTextures[128];
void Render_CharToTextures(int size,
Uint8 fR, Uint8 fG, Uint8 fB,
Uint8 bR, Uint8 bG, Uint8 bB)
{
    SDL_Surface* textSurface;
    TTF_Font* font = TTF_OpenFont("font/BebasNeue-Regular.ttf", size);
    

    SDL_Color foregroundColor = { fR, fG, fB };
    SDL_Color backgroundColor = { bR, bG, bB };

    //std::string chars2 = "♥";
    char chars[1];
    int charssss = 0;
    for( int i = 0; i < 128; i++ ) {
        /// render out characters
        chars[0] = (char)(i);
        textSurface = TTF_RenderText_Blended(font, chars, foregroundColor);
        SDL_Texture* charTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        charTextures[i] = charTexture;
    }
        
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}
//65-90 capitals