#ifndef GRADIENTS_H
#define GRADIENTS_H

#include "pipelinevertex.h"

class Gradients
{
    public:
        Gradients(PipelineVertex *v0, PipelineVertex *v1, PipelineVertex *v2);

        inline float DepthXStep() const { return depth_x_step; }
        inline float DepthYStep() const { return depth_y_step; }
        inline float* DepthValues() { return depth_; }

        virtual ~Gradients();

    protected:

    private:
        PipelineVertex *v0_;
        PipelineVertex *v1_;
        PipelineVertex *v2_;
        float one_over_dx_;
        float one_over_dy_;

        float one_over_w_[3];
        float texcoord_u_[3];
        float texcoord_v_[3];
        float depth_[3];
        float lightamt_[3];

        float CalcXStep(float c0, float c1, float c2);
        float CalcYStep(float c0, float c1, float c2);

        float depth_x_step;
        float depth_y_step;

};

#endif // GRADIENTS_H
