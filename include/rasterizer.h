#ifndef RASTERIZER_H
#define RASTERIZER_H
#include <stdint.h>

#include "edge.h"
#include "global.h"
#include "pipelinevertex.h"
#include "vertex.h"

class Rasterizer
{
    public:
        Rasterizer();
        virtual ~Rasterizer();
        uint32_t * getFramebuffer();
        void Framebuffer(void ** framebufferp) { framebuffer = (uint32_t *) (*framebufferp); }

        void clearFramebuffer();
        void clearZBuffer();

        void rasterize(PipelineVertex *v1, PipelineVertex *v2, PipelineVertex *v3);
        void line(int x0, int y0, int x1, int y1, float zMean);

    protected:

    private:

        inline void fillShape(uint32_t yMin, uint32_t yMax, float meanZ);
        inline void wireframe(uint32_t yMin, uint32_t yMax);
        void inline scanConvertLine(PipelineVertex* minYVert, PipelineVertex* maxYVert, int whichSide);
        void scanConvertTriangle(PipelineVertex *v1, PipelineVertex *v2, PipelineVertex *v3);

        void ScanTriangle(PipelineVertex *vminy, PipelineVertex *vmidy, PipelineVertex *vmaxy);
        void DrawScanLine(Edge *left, Edge *right, uint32_t ycoord);

        static const float EPSILON = 0.00001;
        uint32_t *framebuffer;
        float *zBuffer_;
        uint32_t scanbuffer[VIEWPORT_HEIGHT][2];
};

#endif // RASTERIZER_H
