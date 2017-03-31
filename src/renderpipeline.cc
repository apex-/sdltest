#include "renderpipeline.h"
#include "camera.h"

#include <cassert>

RenderPipeline::RenderPipeline(Camera *camera, Rasterizer *rasterizer)
{
    camera_ = camera;
    rasterizer_ = rasterizer;
}

RenderPipeline::~RenderPipeline()
{
    // dtor
}


void RenderPipeline::Draw(TlcInstance &tlcinstance) {

    TlcPrimitive* primitive = tlcinstance.GetTlcPrimitive();
    Matrix4 vpm = camera_->ViewProjectionMatrix();
    Matrix4 mwt = tlcinstance.Transformation().ToMatrix();
    Matrix4 mvt = vpm * mwt;

    bbclipflags_ = 0;
    bool is_inside_frustrum = CalculateBoundingBoxParameters(primitive->getAabbModelSpace(), mvt);

    if (is_inside_frustrum) {

            vector<Vertex> vertices = primitive->getVertexArray();
            for (uint32_t i=0; i<primitive->getNumberOfVertices(); i++) {

                // Transform vertex position from model-space to view space
                vb_[i].ViewSpacePos(mvt * vertices[i].Pos());
                // Transform normal from model-space to view space
                vb_[i].ViewSpaceNormal(mvt * vertices[i].Normal());
                // Calculate the screen position from the view space position calculated above
                vb_[i].CalcScreenSpacePos();
                // Set the clip flags
                vb_[i].CalcClipFlags(bbclipflags_);
            }

            // assemble, clip (if necessary) and rasterize triangles
            for (uint32_t i=0; i<primitive->getNumberOfIndices(); i+=3) {

                PipelineVertex *pv1 = &vb_[primitive->getIndices()[i]];
                PipelineVertex *pv2 = &vb_[primitive->getIndices()[i+1]];
                PipelineVertex *pv3 = &vb_[primitive->getIndices()[i+2]];

                // back-face culling
                if ((pv3->ScreenSpacePos().x - pv1->ScreenSpacePos().x) * (pv2->ScreenSpacePos().y - pv1->ScreenSpacePos().y) <
                        (pv3->ScreenSpacePos().y - pv1->ScreenSpacePos().y) * (pv2->ScreenSpacePos().x - pv1->ScreenSpacePos().x))
                        continue;

                if (bbclipflags_ == 0) { // object completely inside the view frustrum

                    rasterizer_->rasterize(pv1, pv2, pv3);
                } else {
                    PipelineVertex* pvout[3];
                    PipelineVertex* pvin[3];
                    int nout = 0;
                    int nin = 0;

                    if (pv1->ClipFlags()) { pvout[nout++] = pv1; } else { pvin[nin++] = pv1; }
                    if (pv2->ClipFlags()) { pvout[nout++] = pv2; } else { pvin[nin++] = pv2; }
                    if (pv3->ClipFlags()) { pvout[nout++] = pv3; } else { pvin[nin++] = pv3; }

                    switch(nout) {

                        case 0:
                            rasterizer_->rasterize(pv1, pv2, pv3);
                            break;
                        case 1:
                            {
                                PipelineVertex pc1(*pvout[0]);
                                PipelineVertex pc2(*pvout[0]);
                                ClipLerp(pvout[0], pvin[0], &pc1);
                                ClipLerp(pvout[0], pvin[1], &pc2);
                                pc1.CalcScreenSpacePos();
                                pc2.CalcScreenSpacePos();
                                rasterizer_->rasterize(pvin[0], pvin[1], &pc1);
                                rasterizer_->rasterize(&pc1, &pc2, pvin[1]);
                                break;
                            }
                        case 2:
                            {
                                PipelineVertex pc1(*pvout[0]);
                                PipelineVertex pc2(*pvout[1]);
                                ClipLerp(pvout[0], pvin[0], &pc1);
                                ClipLerp(pvout[1], pvin[0], &pc2);
                                pc1.CalcScreenSpacePos();
                                pc2.CalcScreenSpacePos();
                                rasterizer_->rasterize(&pc1, &pc2, pvin[0]);
                                break;
                            }
                        case 3:
                            // TODO: Actually even if all 3 vertices lie outside the frustrum
                            // they can still "cut" the corners of the frustrum
                            continue;
                    }
                }

            }

            //#ifdef _DEBUG
            DrawBoundingBox();
            //#endif

        } // if(is_inside_frustrum)
}


bool RenderPipeline::ClipLerp(PipelineVertex *pout, PipelineVertex *pin, PipelineVertex *pclip) {

    // Cohen-Sutherland algorithm (3D Version)
    float lerptmp;
    float lerp = 0.0;

    if (pout->ClipFlags() & 0x01) {
        lerptmp = ((-pout->ViewSpacePos().w - pout->ViewSpacePos().x) /
            ((-pout->ViewSpacePos().w - pout->ViewSpacePos().x) - (-pin->ViewSpacePos().w - pin->ViewSpacePos().x)));
        lerp = (lerptmp > lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x02) {
        lerptmp = ((pout->ViewSpacePos().w - pout->ViewSpacePos().x) /
        ((pout->ViewSpacePos().w - pout->ViewSpacePos().x) - (pin->ViewSpacePos().w - pin->ViewSpacePos().x)));
        lerp = (lerptmp > lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x04) {
        lerptmp = ((-pout->ViewSpacePos().w - pout->ViewSpacePos().y) /
        ((-pout->ViewSpacePos().w - pout->ViewSpacePos().y) - (-pin->ViewSpacePos().w - pin->ViewSpacePos().y)));
        lerp = (lerptmp >  lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x08) {
        lerptmp = ((pout->ViewSpacePos().w - pout->ViewSpacePos().y) /
        ((pout->ViewSpacePos().w - pout->ViewSpacePos().y) - (pin->ViewSpacePos().w - pin->ViewSpacePos().y)));
        lerp = (lerptmp >  lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x10) {
        lerptmp = ((pout->ViewSpacePos().w - pout->ViewSpacePos().z) /
        ((pout->ViewSpacePos().w - pout->ViewSpacePos().z) - (pin->ViewSpacePos().w - pin->ViewSpacePos().z)));
        lerp = (lerptmp >  lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x20) {
        lerptmp = ((-pout->ViewSpacePos().w - pout->ViewSpacePos().z) /
        ((-pout->ViewSpacePos().w - pout->ViewSpacePos().z) - (-pin->ViewSpacePos().w - pin->ViewSpacePos().z)));
        lerp = (lerptmp >  lerp) ? lerptmp : lerp;
    }

     pclip->ViewSpacePos(pout->ViewSpacePos().x + lerp * (pin->ViewSpacePos().x - pout->ViewSpacePos().x),
        pout->ViewSpacePos().y + lerp * (pin->ViewSpacePos().y - pout->ViewSpacePos().y),
        pout->ViewSpacePos().z + lerp * (pin->ViewSpacePos().z - pout->ViewSpacePos().z),
        pout->ViewSpacePos().w + lerp * (pin->ViewSpacePos().w - pout->ViewSpacePos().w));
        return true;
}



bool RenderPipeline::CalculateBoundingBoxParameters(Vector4 (&aabb)[8], Matrix4 &mvpm ) {

    int i=0;
    bbclipflags_ = 0;
    uint8_t num_vertices_outside_bb[6] = {0, 0, 0, 0, 0, 0};

    for (i=0; i<8; i++) {
        view_space_aabb_[i] = mvpm * aabb[i];
        if (view_space_aabb_[i].x < -view_space_aabb_[i].w) num_vertices_outside_bb[0]++;
        if (view_space_aabb_[i].x > view_space_aabb_[i].w) num_vertices_outside_bb[1]++;
        if (view_space_aabb_[i].y < -view_space_aabb_[i].w) num_vertices_outside_bb[2]++;
        if (view_space_aabb_[i].y > view_space_aabb_[i].w) num_vertices_outside_bb[3]++;
        if (view_space_aabb_[i].z > view_space_aabb_[i].w) num_vertices_outside_bb[4]++;
        if (view_space_aabb_[i].z < -view_space_aabb_[i].w) num_vertices_outside_bb[5]++;
    }

    for (i=0; i<6; i++) {
        if (num_vertices_outside_bb[i] == 8) {
            return false;
        }
        if (num_vertices_outside_bb[i] > 0) bbclipflags_ |= (1 << i);

    }
    return true;
}


void RenderPipeline::DrawBoundingBox() {

    Vector2 ssaabb[8]; // screen space aa bb

    /// TODO: Clip bounding box correctly
    if (bbclipflags_ > 0) {
        return;
    }

    for (int i=0; i<8; i++) {
        ssaabb[i].x = ((VIEWPORT_WIDTH -1) * ((view_space_aabb_[i].x/view_space_aabb_[i].w) + 1)) / 2.0f;
        ssaabb[i].y = ((VIEWPORT_HEIGHT -1) * -((view_space_aabb_[i].y/view_space_aabb_[i].w) - 1)) / 2.0f;
    }

    // front
    rasterizer_->line(ssaabb[0].x, ssaabb[0].y, ssaabb[1].x, ssaabb[1].y, (view_space_aabb_[0].z + view_space_aabb_[1].z)/2.0);
    rasterizer_->line(ssaabb[1].x, ssaabb[1].y, ssaabb[3].x, ssaabb[3].y, (view_space_aabb_[1].z + view_space_aabb_[3].z)/2.0);
    rasterizer_->line(ssaabb[3].x, ssaabb[3].y, ssaabb[2].x, ssaabb[2].y, (view_space_aabb_[3].z + view_space_aabb_[2].z)/2.0);
    rasterizer_->line(ssaabb[2].x, ssaabb[2].y, ssaabb[0].x, ssaabb[0].y, (view_space_aabb_[2].z + view_space_aabb_[0].z)/2.0);

    // back
    rasterizer_->line(ssaabb[4].x, ssaabb[4].y, ssaabb[5].x, ssaabb[5].y, (view_space_aabb_[4].z + view_space_aabb_[5].z)/2.0);
    rasterizer_->line(ssaabb[5].x, ssaabb[5].y, ssaabb[7].x, ssaabb[7].y, (view_space_aabb_[5].z + view_space_aabb_[7].z)/2.0);
    rasterizer_->line(ssaabb[7].x, ssaabb[7].y, ssaabb[6].x, ssaabb[6].y, (view_space_aabb_[7].z + view_space_aabb_[6].z)/2.0);
    rasterizer_->line(ssaabb[6].x, ssaabb[6].y, ssaabb[4].x, ssaabb[4].y, (view_space_aabb_[6].z + view_space_aabb_[4].z)/2.0);

    // between front/back
    rasterizer_->line(ssaabb[0].x, ssaabb[0].y, ssaabb[4].x, ssaabb[4].y, (view_space_aabb_[0].z + view_space_aabb_[4].z)/2.0);
    rasterizer_->line(ssaabb[1].x, ssaabb[1].y, ssaabb[5].x, ssaabb[5].y, (view_space_aabb_[1].z + view_space_aabb_[5].z)/2.0);
    rasterizer_->line(ssaabb[3].x, ssaabb[3].y, ssaabb[7].x, ssaabb[7].y, (view_space_aabb_[3].z + view_space_aabb_[7].z)/2.0);
    rasterizer_->line(ssaabb[2].x, ssaabb[2].y, ssaabb[6].x, ssaabb[6].y, (view_space_aabb_[2].z + view_space_aabb_[6].z)/2.0);
}
