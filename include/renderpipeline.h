#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include "tlcinstance.h"
#include "camera.h"
#include "pipelinevertex.h"
#include "rasterizer.h"
#include "vertex.h"

#define VERTEX_BUFFER_SIZE 65536

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
        Vector4 view_space_aabb_points_[8];
        uint8_t bbclipflags_; // bounding box clip flags, bit_positions: 0:left, 1:right, 2:bottom, 3:top, 4:near, 5:far

        bool calculateBoundingBoxParameters(Vector4 (&aabb)[8], Matrix4 &mvpm );
        void DrawBoundingBox();
};

#endif // RENDERPIPELINE_H
