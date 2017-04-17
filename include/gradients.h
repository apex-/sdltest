#ifndef GRADIENTS_H
#define GRADIENTS_H

#include "pipelinevertex.h"

class Gradients
{
    public:
        Gradients(PipelineVertex *v0, PipelineVertex *v1, PipelineVertex *v2);
        virtual ~Gradients();

    protected:

    private:
        PipelineVertex *v0_;
        PipelineVertex *v1_;
        PipelineVertex *v2_;
        float one_over_dx_;
        float one_over_dy_;

        float one_over_w_;

        float CalcXStep(float c0, float c1, float c2);
        float CalcYStep(float c0, float c1, float c2);

};

#endif // GRADIENTS_H
