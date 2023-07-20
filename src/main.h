#ifndef MAIN_H
#define MAIN_H
#include <cmath>
typedef enum {
    LEFTSTICK,
    RIGHTSTICK
} CONTROLLERSTICK;

struct V2Int;
struct V2 {
    float x;
    float y;
    V2 operator+(V2 o) {
        V2 v = {x,y};
        v.x += o.x;
        v.y += o.y;
        return v;
    }
    V2 operator+=(V2 o) {
        V2 v = {x,y};
        v.x += o.x;
        v.y += o.y;
        return v;
    }
    /*
    V2 operator+=(V2Int o) {
        V2 v = {x,y};
        v.x += o.x;
        v.y += o.y;
        return v;
    }*/
    V2 operator-(V2 o) {
        V2 v = {x,y};
        v.x -= o.x;
        v.y -= o.y;
        return v;
    }
    V2 operator-=(V2 o) {
        V2 v = {x,y};
        v.x -= o.x;
        v.y -= o.y;
        return v;
    }
    V2 operator*(V2 o) {
        V2 v = {x,y};
        
        v.x *= o.x;
        v.y *= o.y;
        return v;
    }
    V2 operator*(float o) {
        V2 v = {x,y};
        v.x *= o;
        v.y *= o;
        return v;
    }
    V2 operator/(int i) {
        V2 v = {x,y};
        v.x = x/(float)i;
        v.y = y/(float)i;
        return v;
    }
    float Len() {
        return sqrt(x*x + y*y);
    }
    V2 Norm() {
        float len = Len();
        if( len == 0 )
            return {0.0f,0.0f};
        return {x/len,y/len};
    }
    
};

struct V2Int {
    int x;
    int y;
    V2Int operator *(float f) {
        V2Int v = {x,y};
        v.x *= f;
        v.y *= f;
        return v;
    }
    V2Int operator /(int f) {
        V2Int v = {x,y};
        v.x /= f;
        v.y /= f;
        return v;
    }
    V2 operator /(float f) {
        V2 v = {(float)x,(float)y};
        v.x /= f;
        v.y /= f;
        return v;
    }
};

V2 RandV2(int r);
// input
const int TOTAL_KEYS = 256;
extern int keys[256];
extern int keysJustPressed[256];
extern V2 cameraPos;

extern V2 playerDir;
extern V2 playerAim;
extern V2 lastPlayerDir;
extern SDL_Rect mouseR;

extern V2 d, dNorm,mousePos;
extern float len;
extern SDL_Rect textRect; 

extern float del;

/// 0 - playing
/// 1 - paused
extern int gameState;
void Game_Reset();
void GamePause();
void GetVecInfo(float x, float y, float *len, float *normX, float *normY);
#endif