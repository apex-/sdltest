#include <iostream>
#include "Rasterizer.h"


Rasterizer::Rasterizer()
{
    for (int i=0; i<VIEWPORT_HEIGHT; i++) {
        scanbuffer[i][0] = 0;
        scanbuffer[i][1] = 0;
    }

    framebuffer = new uint32_t[VIEWPORT_PIXELS];
    clearFramebuffer();
}


Rasterizer::~Rasterizer()
{

}

uint32_t* Rasterizer::getFramebuffer() {

    return framebuffer;
}


void Rasterizer::clearFramebuffer() {

    for (int i=0; i<VIEWPORT_PIXELS; i++) {
        framebuffer[i] = 0x00000000;
    }
}


void Rasterizer::drawScanBuffer(uint32_t yCoord, uint32_t xMin, uint32_t xMax) {

    scanbuffer[yCoord][0] = xMin;
    scanbuffer[yCoord][1] = xMax;
}


void Rasterizer::fillShape(uint32_t yMin, uint32_t yMax) {

    for (uint32_t j=yMin; j<yMax; j++) {
        for (uint32_t i=scanbuffer[j][0]; i<scanbuffer[j][1]; i++) {
            int pix = j*VIEWPORT_WIDTH+i;
            framebuffer[j*VIEWPORT_WIDTH+i] = 0xffffffff;
        }
    }
}
