#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <iostream>

#include "global.h"
#include "camera.h"
#include "vectors.h"
#include "matrices.h"
#include "rasterizer.h"
#include "quaternion.h"
#include "renderpipeline.h"
#include "tlcinstance.h"
#include "transform.h"
#include "tlcprimitive.h"


using namespace std;

Rasterizer rasterizer;
Camera camera;
RenderPipeline render_pipeline(&camera, &rasterizer);

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;

/**
* Entry point of the application
*/
int main(int argc, char* argv[]) {

    uint32_t i = 0;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // SDL_WINDOW_FULLSCREEN_DESKTOP
    // SDL_WINDOW_SHOWN
    SDL_CreateWindowAndRenderer(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, SDL_WINDOW_SHOWN, &sdlWindow, &sdlRenderer);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(sdlRenderer, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    sdlTexture = SDL_CreateTexture(sdlRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        VIEWPORT_WIDTH, VIEWPORT_HEIGHT);


    TlcPrimitive primitive;
    primitive.loadFromFile("res/mymonkey.obj");

    Vector4 r1(0.3,1.0,0.3,1.0);
    Transform t1;
    Vertex v1in(0.0, 0.5, 0.0);
    Vertex v2in(0.3, -0.5, 0.0);
    Vertex v3in(-0.3, -0.5, 0.0);
    Vertex vTestIn(1500.0, 2000.0, -999.0);
    t1.setPosition(0, 0, 0.0);

    cout << "Vertex test (0,0,-1.0): " << camera.projectionMatrix() * Vertex(0,0,-1.0).Pos() << endl;
    cout << "Vertex test (0,0,-1000.0): " << camera.projectionMatrix() * Vertex(0,0,-1000.0).Pos() << endl;
    cout << "Vertex test (100,100,-100.0): " << camera.projectionMatrix() * Vertex(-100,-100,-1.0).Pos() << endl;
    cout << "Vertex test (0,0,1.0): " << camera.projectionMatrix() * Vertex(0,0,1.0).Pos() << endl;
    cout << "Vertex test (0,0,1000.0): " << camera.projectionMatrix() * Vertex(0,0,1000.0).Pos() << endl;

    t1.setPosition(0,0,-2.2);
   // primitive.getModelWorldTransform().setPosition(0.0, 0.0, -2.7);

    TlcInstance tlcinstance(&primitive);
    tlcinstance.GetTransform().setPosition(0.0, 0.0, -3.0);

    //for (i=0; i<1000; i++) {
    while (true) {
        i++;



            rasterizer.clearFramebuffer();
       rasterizer.clearZBuffer();



        //cout << "MW: " << endl << mw << endl;
        //cout << "VP: " << endl << vp << endl;
        //cout << "MV: " << endl << mvPrimitive << endl;

        //bool boxInsideFrustrum = primitive.isBoxInsideFrustrum(mvPrimitive);
        //cout << "clipFlags: " << (int)clipFlags << endl;
        //if (clipFlags < 128 && clipFlags > 0) {
        //    cout << "NEEDS CLIPPING" << endl;
        //}

        tlcinstance.GetTransform().rot.rotate(r1, i*0.01);
        render_pipeline.Draw(tlcinstance);

        SDL_UpdateTexture(sdlTexture, NULL, (void *)rasterizer.getFramebuffer(), VIEWPORT_WIDTH * sizeof (Uint32));
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);

        if (SDL_QuitRequested()) {
            break;
        }
        SDL_Delay(15);
    }

    SDL_Quit();
    return 0;
}


void setup() {


}

