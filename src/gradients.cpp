#include "gradients.h"

Gradients::Gradients(PipelineVertex *v0, PipelineVertex *v1, PipelineVertex *v2)
{

    v0_ = v0;
    v1_ = v1;
    v2_ = v2;
    one_over_dx_ = 1.0 / ((v1_->ScreenSpacePos().x - v2_->ScreenSpacePos().x) *
        (v0_->ScreenSpacePos().y - v2_->ScreenSpacePos().y) -
        (v0_->ScreenSpacePos().x - v2_->ScreenSpacePos().x) *
        (v1_->ScreenSpacePos().y - v2_->ScreenSpacePos().y));
    one_over_dy_ = -one_over_dx_;

}

Gradients::~Gradients()
{
    //dtor
}


float Gradients::CalcXStep(float c0, float c1, float c2) {

		return (((c1 - c2) * (v0_->ScreenSpacePos().y - v2_->ScreenSpacePos().y)) -
			((c0 - c2) * (v1_->ScreenSpacePos().y - v2_->ScreenSpacePos().y))) * one_over_dx_;
}

float Gradients::CalcYStep(float c0, float c1, float c2) {

		return (((c1 - c2) * (v0_->ScreenSpacePos().x - v2_->ScreenSpacePos().x)) -
			((c0 - c2) * (v1_->ScreenSpacePos().x - v2_->ScreenSpacePos().x))) * one_over_dy_;
}
