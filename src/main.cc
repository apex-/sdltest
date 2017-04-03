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


// Some utility functions (should be moved)
void someVertexTestPoints(Camera &camera);
void HandleUserInput();
void MoveCamera(float dx, float dy, float dz);
void RotateCamera(float angle);
float camer_rotation_angle_ = 0.0;
float cam_xv_ = 0.0;
float cam_yv_ = 0.0;
float cam_zv_ = 0.0;


Rasterizer rasterizer;
Camera camera;
RenderPipeline render_pipeline(&camera, &rasterizer);

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;

SDL_Event event;

bool quit = false;

void *framebuffer;

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


    int pitch = VIEWPORT_WIDTH * sizeof (Uint32);

    TlcPrimitive monkey;
    bool load_success = monkey.loadFromFile("res/mymonkey.obj");
    if (! load_success) {
        return 1;
    }

    TlcPrimitive floor;
    load_success = floor.loadFromFile("res/floor.obj");
    if (! load_success) {
        return 1;
    }


    Vector4 r1(0.0,1.0,0.3,1.0);
    Vector4 r2(0.0,1.0,0.0,1.0);
    someVertexTestPoints(camera);

    TlcInstance monkey_1(&monkey);
    monkey_1.Transformation().setPosition(0.0, 0.0, -4.0);

    TlcInstance monkey_2(&monkey);
    monkey_2.Transformation().setPosition(3.0, -3.0, -5.0);

    TlcInstance monkey_3(&monkey);
    monkey_3.Transformation().setPosition(-5.0, 2.5, -5.0);

    TlcInstance floor_1(&floor);
    floor_1.Transformation().scale(0.3);
    floor_1.Transformation().setPosition(0.0, -5.0, -10.0);

    //Vector4 floor_rot_vect(0.0,1.0,0.0,1.0);

    //floor_1.Transformation().rotate(floor_rot_vect, 50.54);


    while (!quit) {

        HandleUserInput();
        MoveCamera(cam_xv_, cam_yv_, cam_zv_);

        num_frame++;
        rasterizer.clearFramebuffer();
        rasterizer.clearZBuffer();

        monkey_1.Transformation().rot.rotate(r1, num_frame*-0.01);

        // fails
        //monkey_1.Transformation().movePosition(-0.01, 0.01, 0.00);
        monkey_1.Transformation().movePosition(-0.0, 0.0, sin(num_frame*0.1)/10.0);
        monkey_2.Transformation().setPosition(12*sin(num_frame*0.01), 8*sin(num_frame*0.01), -5.0);
        monkey_2.Transformation().rot.rotate(r2, num_frame*0.01);
        monkey_3.Transformation().rot.rotate(r2, num_frame*0.01);

        SDL_LockTexture(sdlTexture, NULL, &framebuffer, &pitch);
        rasterizer.Framebuffer(&framebuffer);

        render_pipeline.Draw(monkey_2);
        render_pipeline.Draw(monkey_3);
        render_pipeline.Draw(monkey_1);
        render_pipeline.Draw(floor_1);

        // OBS: SDL_UpdateTexture() is where most of the time is spend on my Linux Machine with a Nvidia Quadro K2000
        // In this case SDL_UpdateTexture performs calls in libnvidia-glcore.so.340.101
        // SDL_UpdateTexture(sdlTexture, NULL, (void *)rasterizer.getFramebuffer(), VIEWPORT_WIDTH * sizeof (Uint32));

        SDL_UnlockTexture(sdlTexture);
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


void HandleUserInput() {

    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        } else if (event.type == SDL_KEYDOWN) {

            switch(event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    cam_zv_ = -0.1;
                break;
                case SDL_SCANCODE_S:
                    cam_zv_ = 0.1;
                break;
                case SDL_SCANCODE_A:
                    cam_xv_ = -0.1;
                break;
                case SDL_SCANCODE_D:
                    cam_xv_ = 0.1;
                break;
                 case SDL_SCANCODE_U:
                    cam_yv_ = 0.1;
                break;
                 case SDL_SCANCODE_J:
                    cam_yv_ = -0.1;
                    break;
                //    case SDL_SCANCODE_LEFT:
                //    RotateCamera(-0.1);
                //break;
                //    case SDL_SCANCODE_RIGHT:
                //    RotateCamera(0.1);
                //break;
                default:
                    break;

            }
       } else if (event.type == SDL_KEYUP) {

        switch(event.key.keysym.scancode) {

                case SDL_SCANCODE_W:
                    if (cam_zv_ < 0.0f) cam_zv_ = 0.0f;
                    break;
                case SDL_SCANCODE_S:
                    if (cam_zv_ > 0.0f) cam_zv_ = 0.0f;
                    break;
                case SDL_SCANCODE_A:
                    if (cam_xv_ < 0.0f) cam_xv_ = 0.0f;
                    break;
                case SDL_SCANCODE_D:
                    if (cam_xv_ > 0.0f) cam_xv_ = 0.0f;
                    break;
                case SDL_SCANCODE_U:
                    if (cam_yv_ > 0.0f) cam_yv_ = 0.0f;
                    break;
                case SDL_SCANCODE_J:
                    if (cam_yv_ < 0.0f) cam_yv_ = 0.0f;
                    break;
                     //    case SDL_SCANCODE_LEFT:
                //    RotateCamera(-0.1);
                //break;
                //    case SDL_SCANCODE_RIGHT:
                //    RotateCamera(0.1);
                //break;
                                default:
                    break;
            }

        }
    }
}


void MoveCamera(float dx, float dy, float dz) {

    camera.Move(dx, dy, dz);
}


void RotateCamera(float angle) {

    camer_rotation_angle_+=angle;
    camera.Rotate(Vector4(0.0, 1.0, 0.0, 1.0), camer_rotation_angle_);
}



void someVertexTestPoints(Camera &camera) {

    cout << "Some Vertex test points" << endl;
    cout << "Vertex test (0,0,-1.0): " << camera.ProjectionMatrix() * Vertex(0,0,-1.0).Pos() << endl;
    cout << "Vertex test (0,0,-1000.0): " << camera.ProjectionMatrix() * Vertex(0,0,-1000.0).Pos() << endl;
    cout << "Vertex test (100,100,-100.0): " << camera.ProjectionMatrix() * Vertex(-100,-100,-1.0).Pos() << endl;
    cout << "Vertex test (0,0,1.0): " << camera.ProjectionMatrix() * Vertex(0,0,1.0).Pos() << endl;
    cout << "Vertex test (0,0,1000.0): " << camera.ProjectionMatrix() * Vertex(0,0,1000.0).Pos() << endl << endl;
}
