#include "edge.h"
#include <cassert>

Edge::Edge(PipelineVertex *vminy, PipelineVertex *vmaxy)
{
    ystart_ = (int) (ceil(vminy->ScreenSpacePos().y));
    yend_ = (int) (ceil(vmaxy->ScreenSpacePos().y));

    float ydist = vmaxy->ScreenSpacePos().y - vminy->ScreenSpacePos().y;
    float yprestep = ystart_ - vminy->ScreenSpacePos().y;
    xstep_ = (vmaxy->ScreenSpacePos().x - vminy->ScreenSpacePos().x) / ydist;
    x_ = vminy->ScreenSpacePos().x + yprestep * xstep_;
    float xprestep = x_ - vminy->ScreenSpacePos().x;

    // TODO: Debug code
    assert(ystart_ >= 0 && ystart_ <= VIEWPORT_HEIGHT);
    assert(yend_ >= 0 && yend_ <= VIEWPORT_HEIGHT);
}

Edge::~Edge()
{
    //dtor
}


void Edge::Step() {

    assert(ceil(x_) >= 0 && ceil(x_) <= VIEWPORT_WIDTH);
    x_ += xstep_;
}
