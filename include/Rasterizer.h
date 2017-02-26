#ifndef RASTERIZER_H
#define RASTERIZER_H
#include <stdint.h>

#include "global.h"
#include "Vertex.h"
class Rasterizer
{
    public:
        Rasterizer();
        virtual ~Rasterizer();
        uint32_t *getFramebuffer();
        void clearFramebuffer();
        void drawScanBuffer(uint32_t yCoord, uint32_t xMin, uint32_t xMax);
        void fillShape(uint32_t yMin, uint32_t yMax);
        void rasterize(Vertex &v1, Vertex &v2, Vertex &v3);
        void inline scanConvertLine(Vertex& minYVert, Vertex& maxYVert, int whichSide);
        void scanConvertTriangle(Vertex &v1, Vertex &v2, Vertex &v3);
    protected:

    private:
        static const float EPSILON = 0.00001;
        uint32_t *framebuffer;
        uint32_t scanbuffer[VIEWPORT_HEIGHT][2];
};

#endif // RASTERIZER_H
