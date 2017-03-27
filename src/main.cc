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


void someVertexTestPoints(Camera &camera);


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

    uint32_t num_frame = 0;

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

    TlcPrimitive monkey;
    monkey.loadFromFile("res/mymonkey.obj");

    Vector4 r1(0.0,1.0,0.3,1.0);
    Vector4 r2(0.0,1.0,0.0,1.0);
    someVertexTestPoints(camera);

    TlcInstance monkey_1(&monkey);
    monkey_1.Transformation().setPosition(0.0, 0.0, -3.0);

    TlcInstance monkey_2(&monkey);
    monkey_2.Transformation().setPosition(4.0, -3.0, -6.0);

    TlcInstance monkey_3(&monkey);
    monkey_3.Transformation().setPosition(-4.0, 3.0, -6.0);


    while (true) {

        num_frame++;
        rasterizer.clearFramebuffer();
        rasterizer.clearZBuffer();

        monkey_1.Transformation().rot.rotate(r1, num_frame*-0.01);
        //monkey_1.Transformation().movePosition(0.002, 0.0, 0.0);
        monkey_2.Transformation().movePosition(0.01, 0.0, 0.0);
        monkey_2.Transformation().rot.rotate(r2, num_frame*0.01);
        monkey_3.Transformation().rot.rotate(r2, num_frame*0.01);

        render_pipeline.Draw(monkey_1);
        render_pipeline.Draw(monkey_2);
        render_pipeline.Draw(monkey_3);

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


void someVertexTestPoints(Camera &camera) {

    cout << "Some Vertex test points" << endl;
    cout << "Vertex test (0,0,-1.0): " << camera.ProjectionMatrix() * Vertex(0,0,-1.0).Pos() << endl;
    cout << "Vertex test (0,0,-1000.0): " << camera.ProjectionMatrix() * Vertex(0,0,-1000.0).Pos() << endl;
    cout << "Vertex test (100,100,-100.0): " << camera.ProjectionMatrix() * Vertex(-100,-100,-1.0).Pos() << endl;
    cout << "Vertex test (0,0,1.0): " << camera.ProjectionMatrix() * Vertex(0,0,1.0).Pos() << endl;
    cout << "Vertex test (0,0,1000.0): " << camera.ProjectionMatrix() * Vertex(0,0,1000.0).Pos() << endl << endl;

}
