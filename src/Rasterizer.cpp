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

  if (v2.m_pos.y < v1.m_pos.y) {
        Vertex* temp = new Vertex(v1);
        v1 = v2;
        v2 = *temp;
        delete temp;
    }

    if (v3.m_pos.y < v2.m_pos.y) {
        Vertex* temp = new Vertex(v2);
        v2 = v3;
        v3 = *temp;
        delete temp;
    }

    if (v2.m_pos.y < v1.m_pos.y) {
        Vertex* temp = new Vertex(v1);
        v1 = v2;
        v2 = *temp;
        delete temp;
    }

    scanConvertTriangle(v1, v2, v3);
    fillShape(ceil(v1.m_pos.y), ceil(v3.m_pos.y));
}

void Rasterizer::scanConvertTriangle(Vertex &v1, Vertex &v2, Vertex &v3) {

    // order by y coordinate
    if (v2.m_pos.y < v1.m_pos.y) {
        Vertex* temp = new Vertex(v1);
        temp = &v1;
        v1 = v2;
        v2 = *temp;
        delete temp;
    }
    if (v3.m_pos.y < v2.m_pos.y) {
        Vertex* temp = new Vertex(v2);
        v2 = v3;
        v3 = *temp;
        delete temp;
    }
    if (v2.m_pos.y < v1.m_pos.y) {
        Vertex* temp = new Vertex(v1);
        temp = &v1;
        v1 = v2;
        v2 = *temp;
        delete temp;
    }

    float cproduct = v1.m_pos.y * v2.m_pos.x - v1.m_pos.x * v2.m_pos.y;
    int side = cproduct < 0 ? 0 : 1;
    scanConvertLine(v1, v2, side);
    scanConvertLine(v2, v3, side);
    scanConvertLine(v1, v3, 1 - side);
}


void Rasterizer::scanConvertLine(Vertex& vminy, Vertex& vmaxy, int side) {

    int ystart = (int) (ceil(vminy.m_pos.y));
    int yend = (int) (ceil(vmaxy.m_pos.y));
    float ydist = vmaxy.m_pos.y - vminy.m_pos.y;
    if (ydist <= EPSILON) {
        return;
    }
    float xstep = (vmaxy.m_pos.x - vminy.m_pos.x) / ydist;
    float curx = vminy.m_pos.x + (ystart - vminy.m_pos.y) * xstep;

    for (int i=ystart; i<yend; i++) {
        scanbuffer[i][side] = ceil(curx);
        curx += xstep;
    }
}
