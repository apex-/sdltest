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
#include "TLCPrimitive.h"


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


    TLCPrimitive primitive;
    primitive.loadFromFile("res/mymonkey.obj");
    primitive.getModelWorldTransform().setPosition(0.0,0.0,-3.0);

    Camera camera;
    Transform t1;
    Vector4 r1(0.3,1.0,0.0,1.0);

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

     t1.setPosition(0,0,-2.2);

    //for (i=0; i<1000; i++) {
    while (true) {
        i++;
        rasterizer.clearFramebuffer();
        rasterizer.clearZBuffer();

       // camera.pos.set(i*0.01,0.0, i*0.01);

       t1.rotate(r1, i*0.02);

        //cout << " z " << z << endl;

       t1.movePosition(0.0, 0.0, 0.000);

        z-=0.001;

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

        //primitive.modelWorldTransform.setPosition(0.0, 0.0, -30.0);
        //cout << "NOFINDICES " << primitive.getNumberOfIndices() << endl;
        v1.perspectiveDivide();
        v2.perspectiveDivide();
        v3.perspectiveDivide();
        v1.toScreenCoordinates();
        v2.toScreenCoordinates();
        v3.toScreenCoordinates();
        //rasterizer.rasterize(v1, v2, v3);



        primitive.getModelWorldTransform().rot.rotate(r1, 0.01);

        for (int i=0; i<primitive.getNumberOfIndices(); i+=3) {

            Vertex v1pin = primitive.getVertexArray()[primitive.getIndices()[i]];
            Vertex v2pin = primitive.getVertexArray()[primitive.getIndices()[i+1]];
            Vertex v3pin = primitive.getVertexArray()[primitive.getIndices()[i+2]];

            //cout << primitive.getIndices()[i] << endl;
            //cout << primitive.getIndices()[i+1] << endl;
            //cout << primitive.getIndices()[i+2] << endl;
            //cout << endl;


            //cout << v1pin;
            //cout << v2pin;
            //cout << v3pin;


            primitive.getModelWorldTransform().movePosition(0.0,0.0001,-2.00);


           // primitive.getModelWorldTransform().setPosition(0.0, 0.0, 0.0);

            Matrix4 mvpp = primitive.getModelWorldTransform().getTransformation();

           // cout << mvp;

            Vertex v1p(v1pin);
            Vertex v2p(v2pin);
            Vertex v3p(v3pin);


//            cout << v1p;
//            cout << v2p;
//            cout << v3p;

            v1p.m_pos = vp * mvpp * v1pin.m_pos;
            v2p.m_pos = vp * mvpp * v2pin.m_pos;
            v3p.m_pos = vp * mvpp * v3pin.m_pos;


//            cout << v1p;
//            cout << v2p;
//            cout << v3p;

            v1p.perspectiveDivide();
            v2p.perspectiveDivide();
            v3p.perspectiveDivide();

//            cout << v1p;
//            cout << v2p;
//            cout << v3p;

            v1p.toScreenCoordinates();
            v2p.toScreenCoordinates();
            v3p.toScreenCoordinates();

               // cout << "to screen coords " << i << endl;

//            cout << v1p;
//            cout << v2p;
//            cout << v3p;


            if (primitive.isInsideFrustrum(camera) == inside) {

                rasterizer.rasterize(v1p, v2p, v3p);

            }
           // cout << "rasterize " << i << endl;
        }


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

