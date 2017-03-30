#include <iostream>
#include "rasterizer.h"
#include "global.h"

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
    clearFramebuffer();
    clearZBuffer();
}


Rasterizer::~Rasterizer()
{

}

uint32_t* Rasterizer::getFramebuffer() {

    return framebuffer;
}


void Rasterizer::clearFramebuffer() {

    memset(framebuffer, 0, VIEWPORT_PIXELS * sizeof(*framebuffer));

    // This is quite a bit slower on gcc 4.9.2
    //std::fill_n(framebuffer, VIEWPORT_PIXELS, 0);
}

void Rasterizer::clearZBuffer() {

    std::fill_n(zBuffer_, VIEWPORT_PIXELS, std::numeric_limits<float>::max());
}


void Rasterizer::rasterize(PipelineVertex *v1, PipelineVertex *v2, PipelineVertex *v3) {


    if (v2->ScreenSpacePos().y < v1->ScreenSpacePos().y) {
       swap(v1, v2);
    }
    if (v3->ScreenSpacePos().y < v2->ScreenSpacePos().y) {
        swap(v2, v3);
    }
    if (v2->ScreenSpacePos().y < v1->ScreenSpacePos().y) {
        swap(v1, v2);
    }

    float meanZ = (v1->ViewSpacePos().z + v2->ViewSpacePos().z + v3->ViewSpacePos().z) / 3.0;
    scanConvertTriangle(v1, v2, v3);
    fillShape(ceil(v1->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().y), meanZ);
    //wireframe(ceil(v1->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().y));

    // Wireframe using Bresenhams line drawing algorithm
    line(ceil(v1->ScreenSpacePos().x), ceil(v1->ScreenSpacePos().y), ceil(v2->ScreenSpacePos().x), ceil(v2->ScreenSpacePos().y), meanZ);
    line(ceil(v1->ScreenSpacePos().x), ceil(v1->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().x), ceil(v3->ScreenSpacePos().y), meanZ);
    line(ceil(v2->ScreenSpacePos().x), ceil(v2->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().x), ceil(v3->ScreenSpacePos().y), meanZ);
    line(ceil(v2->ScreenSpacePos().x), ceil(v2->ScreenSpacePos().y), ceil(v3->ScreenSpacePos().x), ceil(v3->ScreenSpacePos().y), meanZ);
}


void Rasterizer::scanConvertTriangle(PipelineVertex *vminy, PipelineVertex *vmidy, PipelineVertex *vmaxy) {

    // determine the handedness of the triangle
    float x1 = vmaxy->ScreenSpacePos().x - vminy->ScreenSpacePos().x;
    float y1 = vmaxy->ScreenSpacePos().y - vminy->ScreenSpacePos().y;
    float x2 = vmidy->ScreenSpacePos().x - vminy->ScreenSpacePos().x;
    float y2 = vmidy->ScreenSpacePos().y - vminy->ScreenSpacePos().y;
    float cproduct = (x1 * y2 - x2 * y1);
    int side = cproduct > 0 ? 0 : 1;

    scanConvertLine(vminy, vmidy, side);
    scanConvertLine(vmidy, vmaxy, side);
    scanConvertLine(vminy, vmaxy, 1 - side);
}


void Rasterizer::scanConvertLine(PipelineVertex *vminy, PipelineVertex *vmaxy, int side) {

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


inline void Rasterizer::fillShape(uint32_t yMin, uint32_t yMax, float zMean) {

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


inline void Rasterizer::wireframe(uint32_t yMin, uint32_t yMax) {

    for (uint32_t j=yMin; j<yMax; j++) {
        framebuffer[j*VIEWPORT_WIDTH+scanbuffer[j][0]] = 0xFF999999;
        framebuffer[j*VIEWPORT_WIDTH+scanbuffer[j][1]] = 0xFF999999;
    }
}


/* signum function */
int Rasterizer::sgn(int x){
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}



void Rasterizer::gbham(Vertex &v1, Vertex &v2, float meanZ)
{

    gbham(ceil(v1.Pos().x), ceil(v1.Pos().y), ceil(v2.Pos().x), ceil(v2.Pos().y), meanZ);
}


void Rasterizer::gbham(int xstart,int ystart,int xend,int yend, float zMean)
/*--------------------------------------------------------------
 * Bresenham-Algorithmus: Linien auf Rastergeräten zeichnen
 *
 * Eingabeparameter:
 *    int xstart, ystart        = Koordinaten des Startpunkts
 *    int xend, yend            = Koordinaten des Endpunkts
 *
 * Ausgabe:
 *    void SetPixel(int x, int y) setze ein Pixel in der Grafik
 *         (wird in dieser oder aehnlicher Form vorausgesetzt)
 *---------------------------------------------------------------
 */
{
    int x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;

/* Entfernung in beiden Dimensionen berechnen */
   dx = xend - xstart;
   dy = yend - ystart;

/* Vorzeichen des Inkrements bestimmen */
   incx = sgn(dx);
   incy = sgn(dy);
   if(dx<0) dx = -dx;
   if(dy<0) dy = -dy;

/* feststellen, welche Entfernung größer ist */
   if (dx>dy)
   {
      /* x ist schnelle Richtung */
      pdx=incx; pdy=0;    /* pd. ist Parallelschritt */
      ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
      es =dy;   el =dx;   /* Fehlerschritte schnell, langsam */
   } else
   {
      /* y ist schnelle Richtung */
      pdx=0;    pdy=incy; /* pd. ist Parallelschritt */
      ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
      es =dx;   el =dy;   /* Fehlerschritte schnell, langsam */
   }

/* Initialisierungen vor Schleifenbeginn */
   x = xstart;
   y = ystart;
   err = el/2;

    if (zMean-0.00001 < zBuffer_[y*VIEWPORT_WIDTH+x]) {
        framebuffer[y*VIEWPORT_WIDTH+x] = 0xFFCCCCCC;
        zBuffer_[y*VIEWPORT_WIDTH+x] = zMean;
    }
   //SetPixel(x,y);

/* Pixel berechnen */
   for(t=0; t<el; ++t) /* t zaehlt die Pixel, el ist auch Anzahl */
   {
      /* Aktualisierung Fehlerterm */
      err -= es;
      if(err<0)
      {
          /* Fehlerterm wieder positiv (>=0) machen */
          err += el;
          /* Schritt in langsame Richtung, Diagonalschritt */
          x += ddx;
          y += ddy;
      } else
      {
          /* Schritt in schnelle Richtung, Parallelschritt */
          x += pdx;
          y += pdy;
      }

    if (zMean-0.00001 < zBuffer_[y*VIEWPORT_WIDTH+x]) {
        framebuffer[y*VIEWPORT_WIDTH+x] = 0xFFCCCCCC;
        zBuffer_[y*VIEWPORT_WIDTH+x] = zMean;
    }

      //SetPixel(x,y);
   }
} /* gbham() */


void Rasterizer::line(int x0, int y0, int x1, int y1, float zMean)
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

