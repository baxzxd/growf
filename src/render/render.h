#ifndef RENDER_H
#define RENDER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
using namespace std;

//particles rise up and fade
struct Particle {
    V2 pos;
    float z;
    float zVel;
    V2 vel;
    V2 size;
    int color;
};
struct ParticleSystem {
    V2Int pos;
    Particle particles[16];
    int particleCount;
    float lifetime;
    int color;
    V2Int scale;
};
void Particle_PlayEffect(V2Int pos, int p, int color, V2Int scale);

// rendering
extern SDL_Window *window;
extern SDL_Renderer *renderer;

// graphics
extern SDL_Rect squareRect;
extern TTF_Font* font;
extern SDL_Texture *charTextures[128];

#define WINDOW_TITLE "growf"
#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600
void Render_Main();
void Render_String(string s, V2 pos, int w, int h);
SDL_Texture *Render_PrerenderString(char* s); 
SDL_Texture *Render_PrerenderStringsFromFile(char* path); 
int Render_Init();
void Render_CharToTextures(int size,
Uint8 fR, Uint8 fG, Uint8 fB,
Uint8 bR, Uint8 bG, Uint8 bB);
void Render_SetDrawColor(int c, int alpha);
SDL_Surface *Render_CreateSpeckledSurface(int w, int h, int baseColor, int sColor, int sAmount, int sW, int sH );
SDL_Texture *Render_CreateSpeckledTexture(int w, int h, int baseColor, int sColor, int sAmount, int sW, int sH );
SDL_Texture *Noise_GenerateSimplex(int res, int sc, float freq, float min);
void save_texture(SDL_Renderer *ren, SDL_Texture *tex, const char *filename);
#endif