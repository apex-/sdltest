#ifndef RASTERIZER_H
#define RASTERIZER_H
#include <stdint.h>

#include "edge.h"
#include "global.h"
#include "gradients.h"
#include "pipelinevertex.h"
#include "vertex.h"

class Rasterizer
{
    public:
        Rasterizer();
        virtual ~Rasterizer();
        uint32_t * GetFramebuffer();
        void Framebuffer(void ** framebufferp) { framebuffer = (uint32_t *) (*framebufferp); }

        void ClearFramebuffer();
        void ClearZBuffer();

        void Rasterize(PipelineVertex *v1, PipelineVertex *v2, PipelineVertex *v3);
        void Line(int x0, int y0, int x1, int y1, float zMean);

    protected:

    private:

        inline void FillShape(uint32_t yMin, uint32_t yMax, float meanZ);
        inline void Wireframe(uint32_t yMin, uint32_t yMax);
        void inline ScanConvertLine(PipelineVertex* minYVert, PipelineVertex* maxYVert, int whichSide);
        void ScanConvertTriangle(PipelineVertex *v1, PipelineVertex *v2, PipelineVertex *v3);

        void ScanTriangle(PipelineVertex *vminy, PipelineVertex *vmidy, PipelineVertex *vmaxy);
        void DrawScanLine(Edge *left, Edge *right, Gradients *gradients, uint32_t ycoord);

        static const float EPSILON = 0.00001;
        uint32_t *framebuffer;
        float *zBuffer_;
        uint32_t scanbuffer[VIEWPORT_HEIGHT][2];
};

#endif // RASTERIZER_H
