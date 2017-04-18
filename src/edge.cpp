#include "edge.h"


Edge::Edge(PipelineVertex *left, PipelineVertex *right, Gradients *gradients, uint32_t minyvertidx) {
    ystart_ = (int) (ceil(left->ScreenSpacePos().y));
    yend_ = (int) (ceil(right->ScreenSpacePos().y));

    float ydist = right->ScreenSpacePos().y - left->ScreenSpacePos().y;
    float yprestep = ystart_ - left->ScreenSpacePos().y;
    xstep_ = (right->ScreenSpacePos().x - left->ScreenSpacePos().x) / ydist;
    x_ = left->ScreenSpacePos().x + yprestep * xstep_;
    float xprestep = x_ - left->ScreenSpacePos().x;

    // TODO: Debug code
    assert(ystart_ >= 0 && ystart_ <= VIEWPORT_HEIGHT);
    assert(yend_ >= 0 && yend_ <= VIEWPORT_HEIGHT);

    depth_ = gradients->DepthValues()[minyvertidx] +
        gradients->DepthXStep() * xprestep +
        gradients->DepthYStep() * yprestep;
    depth_step_ = gradients->DepthYStep() + gradients->DepthXStep() * xstep_;
}

Edge::~Edge()
{
    //dtor
}

