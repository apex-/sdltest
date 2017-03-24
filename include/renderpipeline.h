#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include "tlcinstance.h"
#include "camera.h"
#include "pipelinevertex.h"
#include "rasterizer.h"
#include "vertex.h"

#define VERTEX_BUFFER_SIZE 100000

class RenderPipeline
{
    public:
        RenderPipeline();
        virtual ~RenderPipeline();
        void Draw(TlcInstance &tlcinstance);


    protected:

    private:
        Camera camera_;
        Rasterizer rasterizer_;
        PipelineVertex vb_[VERTEX_BUFFER_SIZE];
};

#endif // RENDERPIPELINE_H
