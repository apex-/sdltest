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
#include "transform.h"
#include "tlcprimitive.h"


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

    Vector4 r1(0.3,1.0,0.3,1.0);
    //primitive.getModelWorldTransform().rot.rotate(r1, 3.1415);

    //Vector4 r1(0.0,0.0,1.0,1.0);
    //primitive.getModelWorldTransform().rot.rotate(r1, 3.1415);

    Camera camera;
    Transform t1;
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
   // primitive.getModelWorldTransform().setPosition(0.0, 0.0, -2.7);

    //for (i=0; i<1000; i++) {
    while (true) {
        i++;
        rasterizer.clearFramebuffer();
        rasterizer.clearZBuffer();

        Matrix4 vp = camera.getViewProjection(); // view projection

        // camera.pos.set(i*0.01,0.0, i*0.01);

        //t1.rotate(r1, i*0.02);
        //t1.movePosition(0.0, 0.1, 0.000);
        //z-=0.001;
//        Vertex v1(v1in);
//        Vertex v2(v2in);
//        Vertex v3(v3in);
//        Matrix4 mv = t1.getTransformation(); // model-world transformation
//        Matrix4 mvp = vp * mv; // Model-View Projection
//        v1.m_pos = mvp * v1.m_pos;
//        v2.m_pos = mvp * v2.m_pos;
//        v3.m_pos = mvp * v3.m_pos;
//        v1.perspectiveDivide();
//        v2.perspectiveDivide();
//        v3.perspectiveDivide();
//        v1.toScreenCoordinates();
//        v2.toScreenCoordinates();
//        v3.toScreenCoordinates();
//        rasterizer.rasterize(v1, v2, v3);

        primitive.getModelWorldTransform().rot.rotate(r1, i*0.01);
        //primitive.getModelWorldTransform().movePosition(0.0,0.0,(i%400) > 200 ? 0.01 : -0.01);
        //primitive.getModelWorldTransform().movePosition(0.0,-0.003,0.0);

        Matrix4 mw = primitive.getModelWorldTransform().getTransformation();
        Matrix4 mvPrimitive = vp * mw;
        //cout << "MW: " << endl << mw << endl;
        //cout << "VP: " << endl << vp << endl;
        //cout << "MV: " << endl << mvPrimitive << endl;
        uint8_t clipFlags = primitive.getAabbClipFlags(mvPrimitive);
        //cout << "clipFlags: " << (int)clipFlags << endl;
        if (clipFlags < 128 && clipFlags > 0) {
            cout << "NEEDS CLIPPING" << endl;
        }

        if (clipFlags == 0) {

            for (int i=0; i<primitive.getNumberOfIndices(); i+=3) {

                Vertex v1pin = primitive.getVertexArray()[primitive.getIndices()[i]];
                Vertex v2pin = primitive.getVertexArray()[primitive.getIndices()[i+1]];
                Vertex v3pin = primitive.getVertexArray()[primitive.getIndices()[i+2]];

                Vertex v1p(v1pin);
                Vertex v2p(v2pin);
                Vertex v3p(v3pin);

                v1p.m_pos = mvPrimitive * v1pin.m_pos;
                v2p.m_pos = mvPrimitive * v2pin.m_pos;
                v3p.m_pos = mvPrimitive * v3pin.m_pos;

                v1p.perspectiveDivide();
                v2p.perspectiveDivide();
                v3p.perspectiveDivide();

                v1p.toScreenCoordinates();
                v2p.toScreenCoordinates();
                v3p.toScreenCoordinates();

                rasterizer.rasterize(v1p, v2p, v3p);
            }

                            // draw bounding box
                Vector4 *aabb = primitive.getAabbModelSpace();
                Vertex blf(aabb[0].x, aabb[0].y, aabb[0].z);
                Vertex brf(aabb[1].x, aabb[0].y, aabb[0].z);
                Vertex tlf(aabb[0].x, aabb[1].y, aabb[0].z);
                Vertex trf(aabb[1].x, aabb[1].y, aabb[0].z);
                Vertex blr(aabb[0].x, aabb[0].y, aabb[1].z);
                Vertex brr(aabb[1].x, aabb[0].y, aabb[1].z);
                Vertex tlr(aabb[0].x, aabb[1].y, aabb[1].z);
                Vertex trr(aabb[1].x, aabb[1].y, aabb[1].z);
                blf.m_pos = mvPrimitive * blf.m_pos;
                brf.m_pos = mvPrimitive * brf.m_pos;
                tlf.m_pos = mvPrimitive * tlf.m_pos;
                trf.m_pos = mvPrimitive * trf.m_pos;
                blr.m_pos = mvPrimitive * blr.m_pos;
                brr.m_pos = mvPrimitive * brr.m_pos;
                tlr.m_pos = mvPrimitive * tlr.m_pos;
                trr.m_pos = mvPrimitive * trr.m_pos;

                blf.perspectiveDivide();
                brf.perspectiveDivide();
                tlf.perspectiveDivide();
                trf.perspectiveDivide();
                blr.perspectiveDivide();
                brr.perspectiveDivide();
                tlr.perspectiveDivide();
                trr.perspectiveDivide();

                blf.toScreenCoordinates();
                brf.toScreenCoordinates();
                tlf.toScreenCoordinates();
                trf.toScreenCoordinates();
                blr.toScreenCoordinates();
                brr.toScreenCoordinates();
                tlr.toScreenCoordinates();
                trr.toScreenCoordinates();

                // front
                rasterizer.gbham(blf, brf);
                rasterizer.gbham(brf, trf);
                rasterizer.gbham(trf, tlf);
                rasterizer.gbham(tlf, blf);

                //back
                rasterizer.gbham(blr, brr);
                rasterizer.gbham(brr, trr);
                rasterizer.gbham(trr, tlr);
                rasterizer.gbham(tlr, blr);

                // connections between front and back
                rasterizer.gbham(blf, blr);
                rasterizer.gbham(brf, brr);
                rasterizer.gbham(trf, trr);
                rasterizer.gbham(tlf, tlr);

                //rasterizer.gbham();
        }

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

