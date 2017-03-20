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
    zBuffer = new float[VIEWPORT_PIXELS];
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

    for (uint32_t i=0; i<VIEWPORT_PIXELS; i++) {
        framebuffer[i] = 0x00000000;
    }
}

void Rasterizer::clearZBuffer() {

    for (uint32_t i=0; i<VIEWPORT_PIXELS; i++) {
        zBuffer[i] = 0.0;
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


    // Wireframe using Bresenhams line drawing algorithm
    gbham(ceil(v1.m_pos.x), ceil(v1.m_pos.y), ceil(v2.m_pos.x), ceil(v2.m_pos.y));
    gbham(ceil(v1.m_pos.x), ceil(v1.m_pos.y), ceil(v3.m_pos.x), ceil(v3.m_pos.y));
    gbham(ceil(v2.m_pos.x), ceil(v2.m_pos.y), ceil(v3.m_pos.x), ceil(v3.m_pos.y));

    //scanConvertTriangle(v1, v2, v3);
    //fillShape(ceil(v1.m_pos.y), ceil(v3.m_pos.y));
    //wireframe(ceil(v1.m_pos.y), ceil(v3.m_pos.y));

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
    assert(yend >= 0 && yend <= VIEWPORT_HEIGHT);

    // TODO: REMOVE
//    if (ystart < 0) {
//        ystart = 0;
//    }
//    if (ystart > VIEWPORT_HEIGHT) {
//        ystart = VIEWPORT_HEIGHT;
//    }

    for (uint32_t i=ystart; i<yend; i++) {

        // TODO: REMOVE
//        if (ceil(curx) < 0 && ceil(curx) > VIEWPORT_WIDTH) {
//            continue;
//        }
        // TODO: Debug code
        assert(ceil(curx) >= 0 && ceil(curx) <= VIEWPORT_WIDTH);
        scanbuffer[i][side] = ceil(curx);
        curx += xstep;
    }
}


inline void Rasterizer::fillShape(uint32_t yMin, uint32_t yMax) {

    //int output = min + (rand() % (int)(max - min + 1))

    for (uint32_t j=yMin; j<yMax; j++) {
        for (uint32_t i=scanbuffer[j][0]; i<scanbuffer[j][1]; i++) {
            framebuffer[j*VIEWPORT_WIDTH+i] = 0xff8888ff;
        }
    }
}


inline void Rasterizer::wireframe(uint32_t yMin, uint32_t yMax) {

    //int output = min + (rand() % (int)(max - min + 1))

    for (uint32_t j=yMin; j<yMax; j++) {
        framebuffer[j*VIEWPORT_WIDTH+scanbuffer[j][0]] = 0xAAAAAAAA;
        framebuffer[j*VIEWPORT_WIDTH+scanbuffer[j][1]] = 0xAAAAAAAA;
    }
}


/* signum function */
int Rasterizer::sgn(int x){
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}


void Rasterizer::gbham(int xstart,int ystart,int xend,int yend)
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

   framebuffer[y*VIEWPORT_WIDTH+x] = 0xAAAAAAAA;
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

      framebuffer[y*VIEWPORT_WIDTH+x] = 0xAAAAAAAA;

      //SetPixel(x,y);
   }
} /* gbham() */


