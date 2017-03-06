#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <iostream>

#include "global.h"
#include "Camera.h"
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


    Camera camera;
    Transform t1;

    Vector4 r1(0.0,1,0.5,1);
    t1.setPosition(0, 0, 1.0);

    Vertex v1in(0.0, 0.5, 0.0);
    Vertex v2in(0.3, -0.5, 0.0);
    Vertex v3in(-0.3, -0.5, 0.0);

    float z = 1.0;
    //uint32_t i = 0;
    bool zinc = true;

    //for (i=0; i<1000; i++) {
    while (true) {


        i++;
        t1.rotate(r1, i*0.05);

        cout << " z " << z << endl;

        if (z > 8.0) {
            zinc = false;
        } else if (z < 0.9) {
            zinc = true;
        }

        if (zinc) {
            t1.movePosition(0,0,0.02);
            z+=0.05;
        } else {
            t1.movePosition(0,0,-0.02);
             z-=0.05;
        }

        //t1.setPosition(0, 0, 1.0 + i*0.1);

        Vertex v1(v1in);
        Vertex v2(v2in);
        Vertex v3(v3in);

        Matrix4 mv = t1.getTransformation(); // model-world transformation

        Matrix4 vp = camera.getViewProjection(); // view projection
        Matrix4 mvp = vp * mv; // Model-View Projection

        v1.m_pos = mvp * v1.m_pos;
        v2.m_pos = mvp * v2.m_pos;
        v3.m_pos = mvp * v3.m_pos;

        v1.perspectiveDivide();
        v2.perspectiveDivide();
        v3.perspectiveDivide();

        v1.toScreenCoordinates();
        v2.toScreenCoordinates();
        v3.toScreenCoordinates();

        v1.print();
        v2.print();
        v3.print();

        rasterizer.clearFramebuffer();
        rasterizer.rasterize(v1, v2, v3);

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

