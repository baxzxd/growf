/*
shadows?
render map
put black to light surface
put lights to light surface
render light surface
*/

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "../color.h"
#include "../audio.h"
#include "render.h"

using namespace std;

// rendering
SDL_Window *window;
SDL_Renderer *renderer;

// graphics
SDL_Rect squareRect;
TTF_Font* font;
ParticleSystem particleSystems[128];
SDL_Rect textRect = {120,120,24,30};
SDL_Rect partRect;
void Render_Main() {
    for( int i = 0; i < 128; i++ ) {
        if( particleSystems[i].lifetime <= 0 ) {
            continue;
        }
        for( int j = 0; j < particleSystems[i].particleCount; j++ ) {
            Particle *part = &particleSystems[i].particles[j];

            if( part->stopTime ) {
                if( del > part->stopTime ) {
                    part->pos = part->pos + part->vel * part->stopTime;
                }
                else {
                    part->pos = part->pos + part->vel * del;
                }

                part->stopTime -= del;
                if( part->stopTime <= 0 )
                    part->vel = {0,0};
            }
            V2 p = Render_PosToView(V2{particleSystems[i].pos.x,particleSystems[i].pos.y} + part->pos);
            partRect.x = (int)p.x;
            partRect.y = (int)p.y;
            partRect.w = (int)part->size.x * cameraScale;
            partRect.h = (int)part->size.y * cameraScale;
            Render_SetDrawColor(particleSystems[i].color, 255);
            SDL_RenderFillRect(renderer, &partRect);
        }

        particleSystems[i].lifetime -= del;
    }
}
void Particle_PlayEffect(V2Int pos, int p, int color, V2Int scale) {

    int emptyIndex = -1;
    for( int i = 0; i < 128; i++ ) {
        if( particleSystems[i].lifetime <= 0 ) {
            emptyIndex = i;
            break;
        }
    }

    // probably not needed if childcount is a thing
    if( emptyIndex == -1 )
        return;
    ParticleSystem *ps = &particleSystems[emptyIndex];

    ps->pos = pos;
    ps->color = color;
    ps->lifetime = .3f;
    ps->particleCount = p;
    ps->scale = scale;
    for( int i = 0; i < p; i++ ) {
        ps->particles[i].pos = {0,0};
        ps->particles[i].size = {(float)scale.x,(float)scale.y};
        ps->particles[i].vel = RandV2(150);
        ps->particles[i].stopTime = .15f;
    }
}
void Render_String(string s, V2 pos, int w, int h) {
    textRect.w = w;
    textRect.h = h;
    for( int i = 0; i < s.length(); i++ ) {
        textRect.x = pos.x + textRect.w * i;
        textRect.y = pos.y - (textRect.h + 4);
        SDL_RenderCopy(renderer,charTextures[(int)s[i]], NULL, &textRect);
    }
}
SDL_Rect renderRect;
V2 cameraPos;
float cameraScale = 1.0f;
void Render_RectF( V2 pos, V2 size, Color color ) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    pos = (pos*cameraScale + cameraPos) ;
    size = size * cameraScale;
    renderRect.x = (int)pos.x;
    renderRect.y = (int)pos.y;
    renderRect.w = (int)size.x;
    renderRect.h = (int)size.y;
    SDL_RenderFillRect(renderer, &renderRect);
}
void Render_Rect( V2 pos, V2 size, Color color ) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    pos = pos + cameraPos * cameraScale;
    size = size * cameraScale;
    renderRect.x = (int)pos.x;
    renderRect.y = (int)pos.y;
    renderRect.w = (int)size.x;
    renderRect.h = (int)size.y;
    SDL_RenderDrawRect(renderer, &renderRect);

}
void Render_Copy( SDL_Texture *texture, V2 pos, V2 size ) {
    pos = Render_PosToView(pos);
    size = size * cameraScale;
    renderRect.x = (int)pos.x;
    renderRect.y = (int)pos.y;
    renderRect.w = (int)size.x;
    renderRect.h = (int)size.y;
    SDL_RenderCopy(renderer, texture, NULL, &renderRect);
}
V2 Render_PosToView(V2 pos) {
    return (pos*cameraScale + cameraPos);
}
void Render_SetCameraOffset( V2 pos ) {
    pos = pos * cameraScale;

    if( pos.x > SCREEN_WIDTH/2 ) {
        cameraPos.x =  SCREEN_WIDTH/2 - pos.x;
    }
    if( pos.y > SCREEN_HEIGHT/2 ) {
        cameraPos.y = SCREEN_HEIGHT/2 - pos.y;
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

void save_texture(SDL_Renderer *ren, SDL_Texture *tex, const char *filename)
{
    SDL_Texture *ren_tex;
    SDL_Surface *surf;
    int st;
    int w;
    int h;
    int format;
    void *pixels;

    pixels  = NULL;
    surf    = NULL;
    ren_tex = NULL;
    format  = SDL_PIXELFORMAT_RGBA32;

    /* Get information about texture we want to save */
    st = SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    if (st != 0) {
        SDL_Log("Failed querying texture: %s\n", SDL_GetError());
        goto cleanup;
    }

    ren_tex = SDL_CreateTexture(ren, format, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!ren_tex) {
        SDL_Log("Failed creating render texture: %s\n", SDL_GetError());
        goto cleanup;
    }

    /*
     * Initialize our canvas, then copy texture to a target whose pixel data we 
     * can access
     */
    st = SDL_SetRenderTarget(ren, ren_tex);
    if (st != 0) {
        SDL_Log("Failed setting render target: %s\n", SDL_GetError());
        goto cleanup;
    }

    SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(ren);

    st = SDL_RenderCopy(ren, tex, NULL, NULL);
    if (st != 0) {
        SDL_Log("Failed copying texture data: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Create buffer to hold texture data and load it */
    pixels = malloc(w * h * SDL_BYTESPERPIXEL(format));
    if (!pixels) {
        SDL_Log("Failed allocating memory\n");
        goto cleanup;
    }

    st = SDL_RenderReadPixels(ren, NULL, format, pixels, w * SDL_BYTESPERPIXEL(format));
    if (st != 0) {
        SDL_Log("Failed reading pixel data: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Copy pixel data over to surface */
    surf = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, SDL_BITSPERPIXEL(format), w * SDL_BYTESPERPIXEL(format), format);
    if (!surf) {
        SDL_Log("Failed creating new surface: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Save result to an image */
    st = SDL_SaveBMP(surf, filename);
    if (st != 0) {
        SDL_Log("Failed saving image: %s\n", SDL_GetError());
        goto cleanup;
    }

    SDL_Log("Saved texture as BMP to \"%s\"\n", filename);

cleanup:
    SDL_FreeSurface(surf);
    free(pixels);
    SDL_DestroyTexture(ren_tex);
}
//65-90 capitals