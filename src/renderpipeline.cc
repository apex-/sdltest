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

                vb_[i].ClipFlags(0);

                // Transform Vertex position from model-space to view space
                vb_[i].ViewSpacePos(mvt * vertices[i].Pos());
                // Transform normal from model-space to view space
                vb_[i].ViewSpaceNormal(mvt * vertices[i].Normal());
                // Calculate the screen position from the view space position calculated above
                vb_[i].CalcScreenSpacePos();

                if (bbclipflags_ > 0) { // Check if the bounding-box got clipped

                    // OBS: The additional braces around the conditional is required!
                    if (bbclipflags_ & 0x01) // left plane
                        vb_[i].ClipFlags(vb_[i].ClipFlags() | ((vb_[i].ViewSpacePos().x < -vb_[i].ViewSpacePos().w) ? 0x01 : 0));
                    if (bbclipflags_ & 0x02) // right plane
                        vb_[i].ClipFlags(vb_[i].ClipFlags() | ((vb_[i].ViewSpacePos().x > vb_[i].ViewSpacePos().w) ? 0x02 : 0));
                    if (bbclipflags_ & 0x04) // bottom plane
                        vb_[i].ClipFlags(vb_[i].ClipFlags() | ((vb_[i].ViewSpacePos().y < -vb_[i].ViewSpacePos().w) ? 0x04 : 0));
                    if (bbclipflags_ & 0x08) // top plane
                        vb_[i].ClipFlags(vb_[i].ClipFlags() | ((vb_[i].ViewSpacePos().y > vb_[i].ViewSpacePos().w) ? 0x08 : 0));
                    if (bbclipflags_ & 0x10) // near plane
                        vb_[i].ClipFlags(vb_[i].ClipFlags() | ((vb_[i].ViewSpacePos().z > vb_[i].ViewSpacePos().w) ? 0x10 : 0));
                    if (bbclipflags_ & 0x20) // far plane {
                        vb_[i].ClipFlags(vb_[i].ClipFlags() | ((vb_[i].ViewSpacePos().z < -vb_[i].ViewSpacePos().w) ? 0x20 : 0));
                }
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

                if (bbclipflags_ == 0) {
                    rasterizer_->rasterize(pv1, pv2, pv3);
                } else {
                    PipelineVertex* p_out[3];
                    PipelineVertex* p_in[3];
                    int n_out = 0;
                    int n_in = 0;
                    if (pv1->ClipFlags()) { p_out[n_out++] = pv1; } else { p_in[n_in++] = pv1; }
                    if (pv2->ClipFlags()) { p_out[n_out++] = pv2; } else { p_in[n_in++] = pv2; }
                    if (pv3->ClipFlags()) { p_out[n_out++] = pv3; } else { p_in[n_in++] = pv3; }

                    switch(n_out) {
                        case 0:
                            rasterizer_->rasterize(pv1, pv2, pv3);
                            break;
                        case 1:
                            {
                                PipelineVertex pc1(*p_out[0]);
                                PipelineVertex pc2(*p_out[0]);
                                ClipLerp(p_out[0], p_in[0], &pc1);
                                ClipLerp(p_out[0], p_in[1], &pc2);
                                pc1.CalcScreenSpacePos();
                                pc2.CalcScreenSpacePos();
                                rasterizer_->rasterize(p_in[0], p_in[1], &pc1);
                                rasterizer_->rasterize(&pc1, &pc2, p_in[1]);
                                break;
                            }
                        case 2:
                            {
                                PipelineVertex pc1(*p_out[0]);
                                PipelineVertex pc2(*p_out[1]);
                                ClipLerp(p_out[0], p_in[0], &pc1);
                                ClipLerp(p_out[1], p_in[0], &pc2);
                                pc1.CalcScreenSpacePos();
                                pc2.CalcScreenSpacePos();
                                rasterizer_->rasterize(&pc1, &pc2, p_in[0]);
                                break;
                            }
                        case 3:
                            // TODO: Actually even if all 3 vertices lie outside the frustrum
                            // they can still "cut" the corners of the frustrum
                            continue;

                    }
                }

            }

            #ifdef _DEBUG
            DrawBoundingBox();
            #endif

        } // if(is_inside_frustrum)
}


bool RenderPipeline::ClipLerp(PipelineVertex *pout, PipelineVertex *pin, PipelineVertex *pclip) {

    // Cohen-Sutherland algorithm (3D Version)
    float lerptmp;
    float lerp = 0.0;

    if (pout->ClipFlags() & 0x01) {
        lerptmp = ((-pout->ViewSpacePos().w - pout->ViewSpacePos().x) / ((-pout->ViewSpacePos().w - pout->ViewSpacePos().x) - (-pin->ViewSpacePos().w - pin->ViewSpacePos().x)));
        lerp = (lerptmp > lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x02) {
        lerptmp = ((pout->ViewSpacePos().w - pout->ViewSpacePos().x) / ((pout->ViewSpacePos().w - pout->ViewSpacePos().x) - (pin->ViewSpacePos().w - pin->ViewSpacePos().x)));
        lerp = (lerptmp > lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x04) {
        lerptmp = ((-pout->ViewSpacePos().w - pout->ViewSpacePos().y) / ((-pout->ViewSpacePos().w - pout->ViewSpacePos().y) - (-pin->ViewSpacePos().w - pin->ViewSpacePos().y)));
        lerp = (lerptmp > lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x08) {
        lerptmp = ((pout->ViewSpacePos().w - pout->ViewSpacePos().y) / ((pout->ViewSpacePos().w - pout->ViewSpacePos().y) - (pin->ViewSpacePos().w - pin->ViewSpacePos().y)));
        lerp = (lerptmp > lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x10) {
        lerptmp = ((pout->ViewSpacePos().w - pout->ViewSpacePos().z) / ((pout->ViewSpacePos().w - pout->ViewSpacePos().z) - (pin->ViewSpacePos().w - pin->ViewSpacePos().z)));
        lerp = (lerptmp > lerp) ? lerptmp : lerp;
    }

    if (pout->ClipFlags() & 0x20) {
        lerptmp = ((-pout->ViewSpacePos().w - pout->ViewSpacePos().z) / ((-pout->ViewSpacePos().w - pout->ViewSpacePos().z) - (-pin->ViewSpacePos().w - pin->ViewSpacePos().z)));
        lerp = (lerptmp > lerp) ? lerptmp : lerp;
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
        view_space_aabb_points_[i] = mvpm * aabb[i];
        if (view_space_aabb_points_[i].x < -view_space_aabb_points_[i].w) num_vertices_outside_bb[0]++;
        if (view_space_aabb_points_[i].x > view_space_aabb_points_[i].w) num_vertices_outside_bb[1]++;
        if (view_space_aabb_points_[i].y < -view_space_aabb_points_[i].w) num_vertices_outside_bb[2]++;
        if (view_space_aabb_points_[i].y > view_space_aabb_points_[i].w) num_vertices_outside_bb[3]++;
        if (view_space_aabb_points_[i].z > view_space_aabb_points_[i].w) num_vertices_outside_bb[4]++;
        if (view_space_aabb_points_[i].z < -view_space_aabb_points_[i].w) num_vertices_outside_bb[5]++;
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

    Vector2 screen_space_aabb_points[8];

    /// TODO: Clip bounding box correctly
    if (bbclipflags_ > 0) {
        return;
    }

    for (int i=0; i<8; i++) {
        screen_space_aabb_points[i].x = ((VIEWPORT_WIDTH -1) * ((view_space_aabb_points_[i].x/view_space_aabb_points_[i].w) + 1)) / 2.0f;
        screen_space_aabb_points[i].y = ((VIEWPORT_HEIGHT -1) * -((view_space_aabb_points_[i].y/view_space_aabb_points_[i].w) - 1)) / 2.0f;
    }

    // front
    rasterizer_->gbham(screen_space_aabb_points[0].x, screen_space_aabb_points[0].y, screen_space_aabb_points[1].x, screen_space_aabb_points[1].y, (view_space_aabb_points_[0].z + view_space_aabb_points_[1].z)/2.0);
    rasterizer_->gbham(screen_space_aabb_points[1].x, screen_space_aabb_points[1].y, screen_space_aabb_points[3].x, screen_space_aabb_points[3].y, (view_space_aabb_points_[1].z + view_space_aabb_points_[3].z)/2.0);
    rasterizer_->gbham(screen_space_aabb_points[3].x, screen_space_aabb_points[3].y, screen_space_aabb_points[2].x, screen_space_aabb_points[2].y, (view_space_aabb_points_[3].z + view_space_aabb_points_[2].z)/2.0);
    rasterizer_->gbham(screen_space_aabb_points[2].x, screen_space_aabb_points[2].y, screen_space_aabb_points[0].x, screen_space_aabb_points[0].y, (view_space_aabb_points_[2].z + view_space_aabb_points_[0].z)/2.0);

    // back
    rasterizer_->gbham(screen_space_aabb_points[4].x, screen_space_aabb_points[4].y, screen_space_aabb_points[5].x, screen_space_aabb_points[5].y, (view_space_aabb_points_[4].z + view_space_aabb_points_[5].z)/2.0);
    rasterizer_->gbham(screen_space_aabb_points[5].x, screen_space_aabb_points[5].y, screen_space_aabb_points[7].x, screen_space_aabb_points[7].y, (view_space_aabb_points_[5].z + view_space_aabb_points_[7].z)/2.0);
    rasterizer_->gbham(screen_space_aabb_points[7].x, screen_space_aabb_points[7].y, screen_space_aabb_points[6].x, screen_space_aabb_points[6].y, (view_space_aabb_points_[7].z + view_space_aabb_points_[6].z)/2.0);
    rasterizer_->gbham(screen_space_aabb_points[6].x, screen_space_aabb_points[6].y, screen_space_aabb_points[4].x, screen_space_aabb_points[4].y, (view_space_aabb_points_[6].z + view_space_aabb_points_[4].z)/2.0);

    // between front/back
    rasterizer_->gbham(screen_space_aabb_points[0].x, screen_space_aabb_points[0].y, screen_space_aabb_points[4].x, screen_space_aabb_points[4].y, (view_space_aabb_points_[0].z + view_space_aabb_points_[4].z)/2.0);
    rasterizer_->gbham(screen_space_aabb_points[1].x, screen_space_aabb_points[1].y, screen_space_aabb_points[5].x, screen_space_aabb_points[5].y, (view_space_aabb_points_[1].z + view_space_aabb_points_[5].z)/2.0);
    rasterizer_->gbham(screen_space_aabb_points[3].x, screen_space_aabb_points[3].y, screen_space_aabb_points[7].x, screen_space_aabb_points[7].y, (view_space_aabb_points_[3].z + view_space_aabb_points_[7].z)/2.0);
    rasterizer_->gbham(screen_space_aabb_points[2].x, screen_space_aabb_points[2].y, screen_space_aabb_points[6].x, screen_space_aabb_points[6].y, (view_space_aabb_points_[2].z + view_space_aabb_points_[6].z)/2.0);
}
