#include <iostream>
#include "Rasterizer.h"
#include "global.h"

#include <cassert>
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


void Rasterizer::rasterize(Vertex &v1, Vertex &v2, Vertex &v3) {

    if (v2.m_pos.y < v1.m_pos.y) {
       swap(v1, v2);
    }
    if (v3.m_pos.y < v2.m_pos.y) {
        swap(v2, v3);
    }
    if (v2.m_pos.y < v1.m_pos.y) {
        swap(v1, v2);
    }

    scanConvertTriangle(v1, v2, v3);
    fillShape(ceil(v1.m_pos.y), ceil(v3.m_pos.y));
}


void Rasterizer::scanConvertTriangle(Vertex &vminy, Vertex &vmidy, Vertex &vmaxy) {

    // determine the handedness of the triangle
    float x1 = vmaxy.m_pos.x - vminy.m_pos.x;
    float y1 = vmaxy.m_pos.y - vminy.m_pos.y;
    float x2 = vmidy.m_pos.x - vminy.m_pos.x;
    float y2 = vmidy.m_pos.y - vminy.m_pos.y;
    float cproduct = (x1 * y2 - x2 * y1);
    int side = cproduct > 0 ? 0 : 1;

    scanConvertLine(vminy, vmidy, side);
    scanConvertLine(vmidy, vmaxy, side);
    scanConvertLine(vminy, vmaxy, 1 - side);
}


void Rasterizer::scanConvertLine(Vertex& vminy, Vertex& vmaxy, int side) {

    uint32_t ystart = (int) (ceil(vminy.m_pos.y));
    uint32_t yend = (int) (ceil(vmaxy.m_pos.y));
    float ydist = vmaxy.m_pos.y - vminy.m_pos.y;
    if (ydist <= EPSILON) {
        return;
    }
    float xstep = (vmaxy.m_pos.x - vminy.m_pos.x) / ydist;
    float curx = vminy.m_pos.x + (ystart - vminy.m_pos.y) * xstep;

    // TODO: Debug code
    assert(ystart >= 0 && ystart <= VIEWPORT_HEIGHT);

    for (uint32_t i=ystart; i<yend; i++) {
        scanbuffer[i][side] = ceil(curx);
        // TODO: Debug code
        assert(ceil(curx) >= 0 && ceil(curx) <= VIEWPORT_WIDTH);
        curx += xstep;
    }
}


void Rasterizer::fillShape(uint32_t yMin, uint32_t yMax) {

    for (uint32_t j=yMin; j<yMax; j++) {
        for (uint32_t i=scanbuffer[j][0]; i<scanbuffer[j][1]; i++) {
            framebuffer[j*VIEWPORT_WIDTH+i] = 0xff8888ff;
        }
    }
}
