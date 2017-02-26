#include <iostream>
#include "Rasterizer.h"

#include "math.h"

using namespace std;

Rasterizer::Rasterizer()
{
    for (uint32_t i=0; i<VIEWPORT_HEIGHT; i++) {
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

    for (uint32_t i=0; i<VIEWPORT_PIXELS; i++) {
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
            framebuffer[j*VIEWPORT_WIDTH+i] = 0xffffffff;
        }
    }
}


void Rasterizer::rasterize(Vertex &v1, Vertex &v2, Vertex &v3) {

  if (v2.y < v1.y) {
        Vertex* temp = new Vertex(v1);
        v1 = v2;
        v2 = *temp;
        delete temp;
    }

    if (v3.y < v2.y) {
        Vertex* temp = new Vertex(v2);
        v2 = v3;
        v3 = *temp;
        delete temp;
    }

    if (v2.y < v1.y) {
        Vertex* temp = new Vertex(v1);
        v1 = v2;
        v2 = *temp;
        delete temp;
    }

    scanConvertTriangle(v1, v2, v3);
    fillShape(ceil(v1.y), ceil(v3.y));
}

void Rasterizer::scanConvertTriangle(Vertex &v1, Vertex &v2, Vertex &v3) {

    // order by y coordinate
    if (v2.y < v1.y) {
        Vertex* temp = new Vertex(v1);
        temp = &v1;
        v1 = v2;
        v2 = *temp;
        delete temp;
    }
    if (v3.y < v2.y) {
        Vertex* temp = new Vertex(v2);
        v2 = v3;
        v3 = *temp;
        delete temp;
    }
    if (v2.y < v1.y) {
        Vertex* temp = new Vertex(v1);
        temp = &v1;
        v1 = v2;
        v2 = *temp;
        delete temp;
    }

    float cross = v1.y * v2.x - v1.x * v2.y;

    int side;
    if (cross < 0) {
        side = 0;
    } else {
         side = 1;
    }
    scanConvertLine(v1, v2, side);
    scanConvertLine(v2, v3, side);
    scanConvertLine(v1, v3, 1 - side);
}


void Rasterizer::scanConvertLine(Vertex& vminy, Vertex& vmaxy, int whichSide) {

    int ystart = (int) (ceil(vminy.y));
    int yend = (int) (ceil(vmaxy.y));
    float xdist = vmaxy.x - vminy.x;
    float ydist = vmaxy.y - vminy.y;
    if (ydist <= EPSILON) {
        return;
    }
    float xstep = xdist / ydist;
    float curx = vminy.x + (ystart - vminy.y) * xstep;

    for (int i=ystart; i<yend; i++) {
        scanbuffer[i][whichSide] = ceil(curx);
        curx += xstep;
    }
}
