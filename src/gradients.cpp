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

    // TODO:
    one_over_w_[0] = 1.0/v0_->ViewSpacePos().w;
    one_over_w_[1] = 1.0/v1_->ViewSpacePos().w;
    one_over_w_[2] = 1.0/v2_->ViewSpacePos().w;


    depth_[0] = v0->ViewSpacePos().z;
    depth_[1] = v1->ViewSpacePos().z;
    depth_[2] = v2->ViewSpacePos().z;
    depth_x_step = CalcXStep(depth_[0], depth_[1], depth_[2]);
    depth_y_step = CalcYStep(depth_[0], depth_[1], depth_[2]);
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
