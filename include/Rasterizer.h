#ifndef RASTERIZER_H
#define RASTERIZER_H
#include <stdint.h>

#include "global.h"

class Rasterizer
{
    public:
        Rasterizer();
        virtual ~Rasterizer();
        uint32_t *getFramebuffer();
        void clearFramebuffer();
        void drawScanBuffer(uint32_t yCoord, uint32_t xMin, uint32_t xMax);
        void fillShape(uint32_t yMin, uint32_t yMax);
    protected:

    private:
        uint32_t *framebuffer;
        uint32_t scanbuffer[VIEWPORT_HEIGHT][2];
};

#endif // RASTERIZER_H
