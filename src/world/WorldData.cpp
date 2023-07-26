
#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "world.h"
#include "../main.h"
#include "../render/render.h"
#include "../color.h"
#include "../FastNoiseLite.h"

#include "../obj/ObjMain.h"
#include "../obj/ObjUtil.h"

std::map<std::string, TileData> tileData;
TileData tDa;
void AddTileToData( std::string id, int c, bool flows ) {
    tDa.id = id;
    tDa.c = c;
    tDa.flows = flows;
    tileData[id] = tDa;
}
#include <fstream>

void World_SerializeWorld() {
    // write size of var before var?
    //get 
    ofstream fout;
    fout.open("file.save", ios::out);
    for( int x = 0; x < worldDim * chunkSize; x++ ) {
        for( int y = 0; y < worldDim * chunkSize; y++ ) {
            fout<<World_GetTile(x,y)->data->id;//fout.write((char*) &World_GetTile(x,y)->height, sizeof(World_GetTile(x,y)->height));
        }
    }

    fout.close();
}
void World_DeserializeWorld() {
    ifstream fin;
    fin.open("file.save", ios::in);
    std::cout<<"SAVE"<<fin.is_open()<<std::endl;
    //char* height;
    
    //while( fin >> height ) {
       // std::cout<<height;
        
    //}

    fin.close();
}

// example from https://cplusplus.com/reference/istream/istream/seekg/
// prolly better

//put chunks into regios of like 32
/*
std::ifstream is ("test.txt", std::ifstream::binary);
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);

    // allocate memory:
    char * buffer = new char [length];

    // read data as a block:
    is.read (buffer,length);

    is.close();

    // print content:
    std::cout.write (buffer,length);

    delete[] buffer;
  }
*/