#ifndef EDGE_H
#define EDGE_H

#include "pipelinevertex.h"

class Edge
{
    public:
        Edge(PipelineVertex *vminy, PipelineVertex *vmaxy);
        virtual ~Edge();

        void Step();
        inline uint32_t YStart() { return ystart_; }
        inline uint32_t YEnd() { return yend_; }
        inline uint32_t X() { return x_; }

    protected:

    private:
        uint32_t ystart_;
        uint32_t yend_;
        float x_;
        float xstep_;

};

#endif // EDGE_H
