#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include "tlcinstance.h"
#include "camera.h"
#include "pipelinevertex.h"
#include "rasterizer.h"
#include "vertex.h"

#define VERTEX_BUFFER_SIZE 10000
#define VERTEX_CLIPBUFFER_SIZE 1000

class RenderPipeline
{
    public:
        RenderPipeline(Camera *camera, Rasterizer *rasterizer);
        virtual ~RenderPipeline();
        void Draw(TlcInstance &tlcinstance);

    private:
        Camera *camera_;
        Rasterizer *rasterizer_;

        // The following data will change for every Draw() call
        PipelineVertex vb_[VERTEX_BUFFER_SIZE];
        PipelineVertex vbclip_[VERTEX_CLIPBUFFER_SIZE]; // vertex buffer for clipped vertices
        uint32_t vb_size_;
        uint32_t vbclip_size_; // the number of clip vertices in the vbclip_ vertex buffer
        Vector4 view_space_aabb_[8];
        uint8_t bbclipflags_; // bounding box clip flags, bit_positions: 0:left, 1:right, 2:bottom, 3:top, 4:near, 5:far

        bool ClipLerp(PipelineVertex *pout, PipelineVertex *pin, PipelineVertex *pclip);
        bool ClipLerpVertex(PipelineVertex *pout, PipelineVertex *pin, PipelineVertex *pclip, int iplane);
        bool CalculateBoundingBoxFlags(Vector4 (&aabb)[8], Matrix4 &mvpm );
        void DrawBoundingBox();

};

#endif // RENDERPIPELINE_H
