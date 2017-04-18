#ifndef EDGE_H
#define EDGE_H

#include <cassert>

#include "gradients.h"
#include "pipelinevertex.h"

class Edge
{
    public:
        Edge(PipelineVertex *left, PipelineVertex *right, Gradients *gradients, uint32_t minyvertidx);
        virtual ~Edge();

        inline void Step();
        inline uint32_t YStart() const { return ystart_; }
        inline uint32_t YEnd() const { return yend_; }
        inline uint32_t X() const { return x_; }

        inline float Depth() const { return depth_; }

    protected:

    private:
        uint32_t ystart_;
        uint32_t yend_;
        float x_;
        float xstep_;

        float depth_;
        float depth_step_;

};


inline void Edge::Step() {

    assert(ceil(x_) >= 0 && ceil(x_) <= VIEWPORT_WIDTH);
    x_ += xstep_;
    depth_ += depth_step_;
}


#endif // EDGE_H
