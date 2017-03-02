#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <iostream>

#include "global.h"
#include "Vectors.h"
#include "Matrices.h"
#include "Rasterizer.h"
#include "Quaternion.h"
#include "Transform.h"


using namespace std;

/**
* Entry point of the application
*/
int main(int argc, char* argv[]) {

    uint32_t i = 0;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *sdlWindow;
    SDL_Renderer *sdlRenderer;
    SDL_Texture *sdlTexture;
    Rasterizer rasterizer;

    // SDL_WINDOW_FULLSCREEN_DESKTOP
    // SDL_WINDOW_SHOWN
    SDL_CreateWindowAndRenderer(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, SDL_WINDOW_SHOWN, &sdlWindow, &sdlRenderer);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(sdlRenderer, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    sdlTexture = SDL_CreateTexture(sdlRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    Matrix4 o(1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16);
    Matrix4 p(17,21,25,29,18,22,26,30,19,23,27,31,20,24,28,32);

    Vector4 c(1,2,3,4);

    Matrix4 q = p * o;

    Vector4 r = o * c;

    cout << r;


    Transform t1;

    Vector4 r1(0,0,0,1);


    Vertex v1(50, 50, 0);
    Vertex v2(200, 100, 0);
    Vertex v3(500, 400, 0);

    for (i=0; i<40; i++) {


        t1.rotate(r1, 0.01);
        Matrix4 mv =t1.getTransformation();

        v1.print();
        v2.print();
        v3.print();

        v1.m_pos = mv * v1.m_pos;
        v2.m_pos = mv * v2.m_pos;
        v3.m_pos = mv * v3.m_pos;

        rasterizer.clearFramebuffer();
        rasterizer.rasterize(v3, v2, v1);

        SDL_UpdateTexture(sdlTexture, NULL, (void *)rasterizer.getFramebuffer(), VIEWPORT_WIDTH * sizeof (Uint32));
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);

        if (SDL_QuitRequested()) {
            break;
        }
        SDL_Delay(30);
    }

    SDL_Quit();
    return 0;
}


void setup() {


}

