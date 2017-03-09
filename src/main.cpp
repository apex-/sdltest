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
    Vector4 r1(0.0,1.0,0.0,1.0);

    Vertex v1in(0.0, 0.5, 0.0);
    Vertex v2in(0.3, -0.5, 0.0);
    Vertex v3in(-0.3, -0.5, 0.0);

    Vertex vTestIn(1500.0, 2000.0, -999.0);

    t1.setPosition(0, 0, 0.0);

    float z = -1.01;
    //uint32_t i = 0;
    bool zinc = true;

    cout << "Vertex test (0,0,-1.0): " << camera.getProjectionMatrix() * Vertex(0,0,-1.0).m_pos << endl;
    cout << "Vertex test (0,0,-1000.0): " << camera.getProjectionMatrix() * Vertex(0,0,-1000.0).m_pos << endl;
    cout << "Vertex test (100,100,-100.0): " << camera.getProjectionMatrix() * Vertex(-100,-100,-1.0).m_pos << endl;
    cout << "Vertex test (0,0,1.0): " << camera.getProjectionMatrix() * Vertex(0,0,1.0).m_pos << endl;
    cout << "Vertex test (0,0,1000.0): " << camera.getProjectionMatrix() * Vertex(0,0,1000.0).m_pos << endl;


    //for (i=0; i<1000; i++) {
    while (true) {
        i++;


        camera.pos.set(i*0.01,0.0, i*0.01);


       t1.rotate(r1, i*0.05);

        //cout << " z " << z << endl;

       t1.setPosition(0,0,-1.0);

       //t1.movePosition(0,0,-0.001);

        z-=0.001;
       //t1.movePosition(0,0,-1.01);

//        if (z > 10.0) {
//            zinc = false;
//        } else if (z < 1.2) {
//            zinc = true;
//        }
//
//        if (zinc) {
//            t1.movePosition(0,0,0.01);
//            z+=0.01;
//        } else {
//           t1.movePosition(0,0,-0.01);
//             z-=0.01;
//        }

        //t1.setPosition(0, 0, 1.0 + i*0.1);

        Vertex v1(v1in);
        Vertex v2(v2in);
        Vertex v3(v3in);


        Vertex vTest(vTestIn);


        Matrix4 mv = t1.getTransformation(); // model-world transformation

        Matrix4 vp = camera.getViewProjection(); // view projection

        Matrix4 mvp = vp * mv; // Model-View Projection

        v1.m_pos = mvp * v1.m_pos;
        v2.m_pos = mvp * v2.m_pos;
        v3.m_pos = mvp * v3.m_pos;



        // clipping
        //cout << " perspectiveDivide " << endl;
        //cout << v1; //.print();
        v1.perspectiveDivide();
        //cout << v1;

        //cout << v2;
        v2.perspectiveDivide();
        //cout << v2;

        //cout << v3;
        v3.perspectiveDivide();
        //cout << v3;

        v1.toScreenCoordinates();
        v2.toScreenCoordinates();
        v3.toScreenCoordinates();

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

