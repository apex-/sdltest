#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>

uint32_t *fb; // We think of this as our framebuffer in ARGB8888 format

int main(int argc, char* argv[]) {
    int i =0;

    fb = malloc(640*480*sizeof(uint32_t)); 
    for (i=0;i<640*480;i++) {
        fb[i] = 0x00080055;
    }

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *sdlWindow;
    SDL_Renderer *sdlRenderer;
    SDL_Texture *sdlTexture;

    SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_FULLSCREEN_DESKTOP, &sdlWindow, &sdlRenderer);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(sdlRenderer, 640, 480);

    sdlTexture = SDL_CreateTexture(sdlRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        640, 480);

    for (i=0; i<100; i++) {
        SDL_UpdateTexture(sdlTexture, NULL, fb, 640 * sizeof (Uint32));
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);
        SDL_Delay(30);
    }

    SDL_Quit();
    return 0;
}

