#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <iostream>

#include "global.h"
#include "Vectors.h"
#include "Matrices.h"
#include "Rasterizer.h"


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


    for (i=0; i<400; i++) {

        rasterizer.clearFramebuffer();
        for (int j=200; j<=300; j++) {
            rasterizer.drawScanBuffer(j, 300-j/2.0+i, 300+j/2.0+i);
           // for (int k=100+i*2; k<200+i*2; k++) {
            //    rasterizer.getFramebuffer()[j*VIEWPORT_WIDTH+k] = 0xffffffff;
            //}
        }
        rasterizer.fillShape(200, 300);

        SDL_UpdateTexture(sdlTexture, NULL, (void *)rasterizer.getFramebuffer(), VIEWPORT_WIDTH * sizeof (Uint32));
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);
        SDL_Delay(30);
    }

    SDL_Quit();
    return 0;
}

