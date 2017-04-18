#include <iostream>
#include "edge.h"
#include "rasterizer.h"
#include "global.h"
#include "gradients.h"

#include <cassert>
#include <limits>
#include <cstring>
#include "math.h"

using namespace std;

Rasterizer::Rasterizer()
{
    for (uint32_t i=0; i<VIEWPORT_HEIGHT; i++) {
        scanbuffer[i][0] = 0;
        scanbuffer[i][1] = 0;
    }

    framebuffer = new uint32_t[VIEWPORT_PIXELS];
    zBuffer_ = new float[VIEWPORT_PIXELS];
    ClearFramebuffer();
    ClearZBuffer();
}


Rasterizer::~Rasterizer()
{

}

uint32_t* Rasterizer::GetFramebuffer() {

    return framebuffer;
}


void Rasterizer::ClearFramebuffer() {

    memset(framebuffer, 0, VIEWPORT_PIXELS * sizeof(*framebuffer));

    // This is quite a bit slower on gcc 4.9.2
    //std::fill_n(framebuffer, VIEWPORT_PIXELS, 0);
}

void Rasterizer::ClearZBuffer() {

    std::fill_n(zBuffer_, VIEWPORT_PIXELS, std::numeric_limits<float>::max());
}


void Rasterizer::Rasterize(PipelineVertex *v1, PipelineVertex *v2, PipelineVertex *v3) {


    if (v2->ScreenSpacePos().y < v1->ScreenSpacePos().y) {
       swap(v1, v2);
    }
    if (v3->ScreenSpacePos().y < v2->ScreenSpacePos().y) {
        swap(v2, v3);
    }
    if (v2->ScreenSpacePos().y < v1->ScreenSpacePos().y) {
        swap(v1, v2);
    }

    float frontz = std::min(v1->ViewSpacePos().z, v2->ViewSpacePos().z);
    frontz = std::min(frontz, v3->ViewSpacePos().z);

    ScanTriangle(v1, v2, v3);
    //ScanConvertTriangle(v1, v2, v3);
    //FillShape(ceil(v1->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().y), meanz);
    //Wireframe(ceil(v1->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().y));

    // Wireframe using Bresenhams line drawing algorithm
//    Line(ceil(v1->ScreenSpacePos().x), ceil(v1->ScreenSpacePos().y), ceil(v2->ScreenSpacePos().x), ceil(v2->ScreenSpacePos().y), meanz);
//    Line(ceil(v1->ScreenSpacePos().x), ceil(v1->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().x), ceil(v3->ScreenSpacePos().y), meanz);
//    Line(ceil(v2->ScreenSpacePos().x), ceil(v2->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().x), ceil(v3->ScreenSpacePos().y), meanz);
//    Line(ceil(v2->ScreenSpacePos().x), ceil(v2->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().x), ceil(v3->ScreenSpacePos().y), meanz);

    // OBS: No ceiling function here otherwise we get out of bounds on y_max, x_max
    Line(v1->ScreenSpacePos().x, v1->ScreenSpacePos().y, v2->ScreenSpacePos().x, v2->ScreenSpacePos().y, frontz);
    Line(v1->ScreenSpacePos().x, v1->ScreenSpacePos().y, v3->ScreenSpacePos().x, v3->ScreenSpacePos().y, frontz);
    Line(v2->ScreenSpacePos().x, v2->ScreenSpacePos().y, v3->ScreenSpacePos().x, v3->ScreenSpacePos().y, frontz);
    Line(v2->ScreenSpacePos().x, v2->ScreenSpacePos().y, v3->ScreenSpacePos().x, v3->ScreenSpacePos().y, frontz);
}


void Rasterizer::ScanTriangle(PipelineVertex *vminy, PipelineVertex *vmidy, PipelineVertex *vmaxy) {

    // determine the handedness of the triangle
    float x1 = vmaxy->ScreenSpacePos().x - vminy->ScreenSpacePos().x;
    float y1 = vmaxy->ScreenSpacePos().y - vminy->ScreenSpacePos().y;
    float x2 = vmidy->ScreenSpacePos().x - vminy->ScreenSpacePos().x;
    float y2 = vmidy->ScreenSpacePos().y - vminy->ScreenSpacePos().y;
    float cproduct = (x1 * y2 - x2 * y1);
    bool is_left_side = cproduct > 0 ? true : false;

    Gradients gradients(vminy, vmidy, vmaxy);

    Edge top_bottom_edge(vminy, vmaxy, &gradients, 0);
    Edge top_mid_edge(vminy, vmidy, &gradients, 0);
    Edge mid_bottom_edge(vmidy, vmaxy, &gradients, 1);

    Edge *left = &top_bottom_edge;
    Edge *right = &top_mid_edge;
    if (is_left_side) {
        std::swap(left, right);
    }
    for (int ycoord=top_mid_edge.YStart(); ycoord<top_mid_edge.YEnd(); ycoord++) {
        DrawScanLine(left, right, &gradients, ycoord);
        left->Step();
        right->Step();
    }

    left = &top_bottom_edge;
    right = &mid_bottom_edge;
    if (is_left_side) {
        std::swap(left, right);
    }
    for (int ycoord=mid_bottom_edge.YStart(); ycoord<mid_bottom_edge.YEnd(); ycoord++) {
        DrawScanLine(left, right, &gradients, ycoord);
        left->Step();
        right->Step();
    }
}


void Rasterizer::DrawScanLine(Edge *left, Edge *right, Gradients *gradients, uint32_t ycoord) {

    uint32_t xmin = ceil(left->X());
    uint32_t xmax = ceil(right->X());
    float xprestep = xmin-left->X();

    //float zMean =

    float depth = left->Depth() + gradients->DepthXStep() * xprestep;

    //cout << "From " << xmin << " to " << xmax << endl;
     for (uint32_t i=xmin; i<xmax; i++) {

            if (depth < zBuffer_[ycoord*VIEWPORT_WIDTH+i]) {
                framebuffer[ycoord*VIEWPORT_WIDTH+i] = 0xff333377;
                zBuffer_[ycoord*VIEWPORT_WIDTH+i] = depth;
            }
            depth += gradients->DepthXStep();
    }

}


void Rasterizer::ScanConvertTriangle(PipelineVertex *vminy, PipelineVertex *vmidy, PipelineVertex *vmaxy) {

    // determine the handedness of the triangle
    float x1 = vmaxy->ScreenSpacePos().x - vminy->ScreenSpacePos().x;
    float y1 = vmaxy->ScreenSpacePos().y - vminy->ScreenSpacePos().y;
    float x2 = vmidy->ScreenSpacePos().x - vminy->ScreenSpacePos().x;
    float y2 = vmidy->ScreenSpacePos().y - vminy->ScreenSpacePos().y;
    float cproduct = (x1 * y2 - x2 * y1);
    int side = cproduct > 0 ? 0 : 1;

    ScanConvertLine(vminy, vmidy, side);
    ScanConvertLine(vmidy, vmaxy, side);
    ScanConvertLine(vminy, vmaxy, 1 - side);
}


void Rasterizer::ScanConvertLine(PipelineVertex *vminy, PipelineVertex *vmaxy, int side) {

    uint32_t ystart = (int) (ceil(vminy->ScreenSpacePos().y));
    uint32_t yend = (int) (ceil(vmaxy->ScreenSpacePos().y));
    float ydist = vmaxy->ScreenSpacePos().y - vminy->ScreenSpacePos().y;
    if (ydist <= EPSILON) {
        return;
    }

    float xstep = (vmaxy->ScreenSpacePos().x - vminy->ScreenSpacePos().x) / ydist;
    float curx = vminy->ScreenSpacePos().x + (ystart - vminy->ScreenSpacePos().y) * xstep;

    // TODO: Debug code
    assert(ystart >= 0 && ystart <= VIEWPORT_HEIGHT);
    assert(yend >= 0 && yend <= VIEWPORT_HEIGHT);

    for (uint32_t i=ystart; i<yend; i++) {
        assert(ceil(curx) >= 0 && ceil(curx) <= VIEWPORT_WIDTH);
        scanbuffer[i][side] = ceil(curx);
        curx += xstep;
    }
}


inline void Rasterizer::FillShape(uint32_t yMin, uint32_t yMax, float zMean) {

    //int output = min + (rand() % (int)(max - min + 1))

    for (uint32_t j=yMin; j<yMax; j++) {
        for (uint32_t i=scanbuffer[j][0]; i<scanbuffer[j][1]; i++) {
            if (zMean < zBuffer_[j*VIEWPORT_WIDTH+i]) {
                framebuffer[j*VIEWPORT_WIDTH+i] = 0xff333377;
                zBuffer_[j*VIEWPORT_WIDTH+i] = zMean;
            }
        }
    }
}


inline void Rasterizer::Wireframe(uint32_t yMin, uint32_t yMax) {

    for (uint32_t j=yMin; j<yMax; j++) {
        framebuffer[j*VIEWPORT_WIDTH+scanbuffer[j][0]] = 0xFF999999;
        framebuffer[j*VIEWPORT_WIDTH+scanbuffer[j][1]] = 0xFF999999;
    }
}


void Rasterizer::Line(int x0, int y0, int x1, int y1, float zMean)
{
  int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2; /* error value e_xy */

  while(1){
    if (zMean-0.00001 < zBuffer_[y0*VIEWPORT_WIDTH+x0]) {
        framebuffer[y0*VIEWPORT_WIDTH+x0] = 0xFFCCCCCC;
        zBuffer_[y0*VIEWPORT_WIDTH+x0] = zMean;
    }
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }
}

