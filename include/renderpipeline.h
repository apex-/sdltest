#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include "tlcinstance.h"
#include "camera.h"
#include "rasterizer.h"

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

};

#endif // RENDERPIPELINE_H
