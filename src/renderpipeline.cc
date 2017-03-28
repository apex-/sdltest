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
                    if (bbclipflags_ & 0x01) // left plane
                        vb_[i].ClipFlags((vb_[i].ClipFlags() | (vb_[i].ViewSpacePos().x < -vb_[i].ViewSpacePos().w) ? 0x01 : 0));
                    if (bbclipflags_ & 0x02) // right plane
                        vb_[i].ClipFlags(vb_[i].ClipFlags() | (vb_[i].ViewSpacePos().x > vb_[i].ViewSpacePos().w) ? 0x02 : 0);
                    if (bbclipflags_ & 0x04) // bottom plane
                        vb_[i].ClipFlags((vb_[i].ClipFlags() | (vb_[i].ViewSpacePos().y < -vb_[i].ViewSpacePos().w) ? 0x04 : 0));
                    if (bbclipflags_ & 0x08) // top plane
                        vb_[i].ClipFlags(vb_[i].ClipFlags() | (vb_[i].ViewSpacePos().y > vb_[i].ViewSpacePos().w) ? 0x08 : 0);
                    if (bbclipflags_ & 0x10) // near plane
                        vb_[i].ClipFlags((vb_[i].ClipFlags() | (vb_[i].ViewSpacePos().z > vb_[i].ViewSpacePos().w) ? 0x10 : 0));
                    if (bbclipflags_ & 0x20) // far plane
                        vb_[i].ClipFlags(vb_[i].ClipFlags() | (vb_[i].ViewSpacePos().z < -vb_[i].ViewSpacePos().w) ? 0x20 : 0);
                }
            }

            // assemble and rasterize triangles
            for (uint32_t i=0; i<primitive->getNumberOfIndices(); i+=3) {

                PipelineVertex *pv1 = &vb_[primitive->getIndices()[i]];
                PipelineVertex *pv2 = &vb_[primitive->getIndices()[i+1]];
                PipelineVertex *pv3 = &vb_[primitive->getIndices()[i+2]];

                // back-face culling
                if ((pv3->ScreenSpacePos().x - pv1->ScreenSpacePos().x) * (pv2->ScreenSpacePos().y - pv1->ScreenSpacePos().y) <
                        (pv3->ScreenSpacePos().y - pv1->ScreenSpacePos().y) * (pv2->ScreenSpacePos().x - pv1->ScreenSpacePos().x))
                        continue;

//                if (pv1->ClipFlags() & pv2->ClipFlags() & pv3->ClipFlags()) {
//                    continue;
//                }


                if (pv1->ClipFlags() | pv2->ClipFlags() | pv3->ClipFlags()) {

                    PipelineVertex* p_out[3];
                    PipelineVertex* p_in[3];
                    int n_out = 0;
                    int n_in = 0;

                    if (pv1->ClipFlags()) { p_out[n_out++] = pv1; } else { p_in[n_in++] = pv1; }
                    if (pv2->ClipFlags()) { p_out[n_out++] = pv2; } else { p_in[n_in++] = pv2; }
                    if (pv3->ClipFlags()) { p_out[n_out++] = pv3; } else { p_in[n_in++] = pv3; }

                    if (n_out == 3) {

                        continue;

                    } else if (n_out == 2) {

                        cout << "in n == 2" << endl;
                        PipelineVertex pc1(*p_out[0]);
                        PipelineVertex pc2(*p_out[1]);
                        ClipLerp(p_out[0], p_in[0], &pc1);
                        ClipLerp(p_out[1], p_in[0], &pc2);
                        pc1.CalcScreenSpacePos();
                        pc2.CalcScreenSpacePos();

                        //rasterizer_->rasterize(&pc1, &pc2, p_in[0]);

                    } else if (n_out == 1) { // n_out == 1

                        PipelineVertex pc1(*p_out[0]);
                        PipelineVertex pc2(*p_out[0]);



                        //cout << "p_out[0] addr " << p_out[0] << endl;
                        //cout << "p_in[0] addr " << p_in[0] << endl;

                        //cout << " p_out[0] clipflags: " << (int)p_out[0]->ClipFlags() << endl;
                        //cout << " p_in[0] clipflags: " << (int)p_in[0]->ClipFlags() << endl;
                        //cout << " p_in[1] clipflags: " << (int)p_in[1]->ClipFlags() << endl;
                        cout << endl;

                        ClipLerp(p_out[0], p_in[0], &pc1);
                        ClipLerp(p_out[0], p_in[1], &pc2);

                        cout << "N-OUT1: p_out[0] " << p_out[0]->ViewSpacePos() << endl;
                        cout << "N-OUT1: p_in[0] " << p_in[0]->ViewSpacePos() << endl;
                        cout << "N-OUT1: p_in[1] " << p_in[1]->ViewSpacePos() << endl;
                        cout << "N-OUT1: pc1 " << pc1.ViewSpacePos() << endl;
                        cout << "N-OUT1: pc2 " << pc2.ViewSpacePos() << endl;
                        cout << endl;

                        pc1.CalcScreenSpacePos();
                        pc2.CalcScreenSpacePos();

                         rasterizer_->rasterize(p_in[0], p_in[1], &pc1);
                         rasterizer_->rasterize(&pc1, &pc2, p_in[1]);
                    }

                } else {
                    rasterizer_->rasterize(pv1, pv2, pv3);
                }


                //#ifdef _DEBUG
                //DrawBoundingBox();
                //#endif
            }
   }

}


bool RenderPipeline::ClipLerp(PipelineVertex *pout, PipelineVertex *pin, PipelineVertex *pclip) {

     //cout << "HELLOOOOOO" << endl << endl;

     // cout << "cliplerp p_out[0] " << pout << endl;
     // cout << "cliplerp p_in[0] " << pin << endl;

    // Cohen-Sutherland algorithm (3D Version)
    float clipvalue;

    //assert(abs(pin->ViewSpacePos().x - pout->ViewSpacePos().x) > 0.0000001);

    if (pout->ClipFlags() & 0x01) {
        clipvalue = -pout->ViewSpacePos().w;
            pclip->ViewSpacePos(clipvalue,
            pout->ViewSpacePos().y + ((clipvalue - pout->ViewSpacePos().x) * (pin->ViewSpacePos().y - pout->ViewSpacePos().y) / ((pout->ViewSpacePos().x - pout->ViewSpacePos().w) - (pin->ViewSpacePos().x - pin->ViewSpacePos().w))),
            pout->ViewSpacePos().z + ((clipvalue - pout->ViewSpacePos().x) * (pin->ViewSpacePos().z - pout->ViewSpacePos().z) / ((pout->ViewSpacePos().x - pout->ViewSpacePos().w) - (pin->ViewSpacePos().x - pin->ViewSpacePos().w))),
            pout->ViewSpacePos().w + ((clipvalue - pout->ViewSpacePos().x) * (pin->ViewSpacePos().w - pout->ViewSpacePos().w) / ((pout->ViewSpacePos().x - pout->ViewSpacePos().w) - (pin->ViewSpacePos().x - pin->ViewSpacePos().w))));
        return true;

    } else if (pout->ClipFlags() & 0x02) {
        clipvalue = pout->ViewSpacePos().w;
        pclip->ViewSpacePos(clipvalue,

            pout->ViewSpacePos().y + ((clipvalue - pout->ViewSpacePos().x) * (pin->ViewSpacePos().y - pout->ViewSpacePos().y) / ((pout->ViewSpacePos().x - pout->ViewSpacePos().w) - (pin->ViewSpacePos().x - pin->ViewSpacePos().w))),
            pout->ViewSpacePos().z + ((clipvalue - pout->ViewSpacePos().x) * (pin->ViewSpacePos().z - pout->ViewSpacePos().z) / ((pout->ViewSpacePos().x - pout->ViewSpacePos().w) - (pin->ViewSpacePos().x - pin->ViewSpacePos().w))),
            pout->ViewSpacePos().w + ((clipvalue - pout->ViewSpacePos().x) * (pin->ViewSpacePos().w - pout->ViewSpacePos().w) / ((pout->ViewSpacePos().x - pout->ViewSpacePos().w) - (pin->ViewSpacePos().x - pin->ViewSpacePos().w))));
        return true;

    } else if (pout->ClipFlags() & 0x04) {
        clipvalue = -pout->ViewSpacePos().w;
        pclip->ViewSpacePos(
            pout->ViewSpacePos().x + ((clipvalue - pout->ViewSpacePos().y) * (pin->ViewSpacePos().x - pout->ViewSpacePos().x) / ((pout->ViewSpacePos().y - pout->ViewSpacePos().w) - (pin->ViewSpacePos().y - pin->ViewSpacePos().w))),
            clipvalue,
            pout->ViewSpacePos().z + ((clipvalue - pout->ViewSpacePos().y) * (pin->ViewSpacePos().z - pout->ViewSpacePos().z) / ((pout->ViewSpacePos().y - pout->ViewSpacePos().w) - (pin->ViewSpacePos().y - pin->ViewSpacePos().w))),
            pout->ViewSpacePos().w + ((clipvalue - pout->ViewSpacePos().y) * (pin->ViewSpacePos().w - pout->ViewSpacePos().w) / ((pout->ViewSpacePos().y - pout->ViewSpacePos().w) - (pin->ViewSpacePos().y - pin->ViewSpacePos().w))));
        return true;

    } else if (pout->ClipFlags() & 0x08) {

        clipvalue = pout->ViewSpacePos().w;
        pclip->ViewSpacePos(
            pout->ViewSpacePos().x + ((clipvalue - pout->ViewSpacePos().y) * (pin->ViewSpacePos().x - pout->ViewSpacePos().x) / ((pout->ViewSpacePos().y - pout->ViewSpacePos().w) - (pin->ViewSpacePos().y - pin->ViewSpacePos().w))),
            clipvalue,
            pout->ViewSpacePos().z + ((clipvalue - pout->ViewSpacePos().y) * (pin->ViewSpacePos().z - pout->ViewSpacePos().z) / ((pout->ViewSpacePos().y - pout->ViewSpacePos().w) - (pin->ViewSpacePos().y - pin->ViewSpacePos().w))),
            pout->ViewSpacePos().w + ((clipvalue - pout->ViewSpacePos().y) * (pin->ViewSpacePos().w - pout->ViewSpacePos().w) / ((pout->ViewSpacePos().y - pout->ViewSpacePos().w) - (pin->ViewSpacePos().y - pin->ViewSpacePos().w))));
        return true;


    } else if (pout->ClipFlags() & 0x01) {
        clipvalue = pout->ViewSpacePos().w;
        pclip->ViewSpacePos(
            pout->ViewSpacePos().x + ((clipvalue - pout->ViewSpacePos().z) * (pin->ViewSpacePos().x - pout->ViewSpacePos().x) / ((pout->ViewSpacePos().z - pout->ViewSpacePos().w) - (pin->ViewSpacePos().z - pin->ViewSpacePos().w))),
            pout->ViewSpacePos().y + ((clipvalue - pout->ViewSpacePos().z) * (pin->ViewSpacePos().y - pout->ViewSpacePos().y) / ((pout->ViewSpacePos().z - pout->ViewSpacePos().w) - (pin->ViewSpacePos().z - pin->ViewSpacePos().w))),
            clipvalue,
            pout->ViewSpacePos().w + ((clipvalue - pout->ViewSpacePos().z) * (pin->ViewSpacePos().w - pout->ViewSpacePos().w) / ((pout->ViewSpacePos().z - pout->ViewSpacePos().w) - (pin->ViewSpacePos().z - pin->ViewSpacePos().w))));
        return true;

    } else if (pout->ClipFlags() & 0x02) {
        clipvalue = -pout->ViewSpacePos().w;
        pclip->ViewSpacePos(
            pout->ViewSpacePos().x + ((clipvalue - pout->ViewSpacePos().z) * (pin->ViewSpacePos().x - pout->ViewSpacePos().x) / ((pout->ViewSpacePos().z - pout->ViewSpacePos().w) - (pin->ViewSpacePos().z - pin->ViewSpacePos().w))),
            pout->ViewSpacePos().y + ((clipvalue - pout->ViewSpacePos().z) * (pin->ViewSpacePos().y - pout->ViewSpacePos().y) / ((pout->ViewSpacePos().z - pout->ViewSpacePos().w) - (pin->ViewSpacePos().z - pin->ViewSpacePos().w))),
            clipvalue,
            pout->ViewSpacePos().w + ((clipvalue - pout->ViewSpacePos().z) * (pin->ViewSpacePos().w - pout->ViewSpacePos().w) / ((pout->ViewSpacePos().z - pout->ViewSpacePos().w) - (pin->ViewSpacePos().z - pin->ViewSpacePos().w))));
        return true;
    }
     return false;

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
    rasterizer_->gbham(screen_space_aabb_points[0].x, screen_space_aabb_points[0].y, screen_space_aabb_points[1].x, screen_space_aabb_points[1].y);
    rasterizer_->gbham(screen_space_aabb_points[1].x, screen_space_aabb_points[1].y, screen_space_aabb_points[3].x, screen_space_aabb_points[3].y);
    rasterizer_->gbham(screen_space_aabb_points[3].x, screen_space_aabb_points[3].y, screen_space_aabb_points[2].x, screen_space_aabb_points[2].y);
    rasterizer_->gbham(screen_space_aabb_points[2].x, screen_space_aabb_points[2].y, screen_space_aabb_points[0].x, screen_space_aabb_points[0].y);

    // back
    rasterizer_->gbham(screen_space_aabb_points[4].x, screen_space_aabb_points[4].y, screen_space_aabb_points[5].x, screen_space_aabb_points[5].y);
    rasterizer_->gbham(screen_space_aabb_points[5].x, screen_space_aabb_points[5].y, screen_space_aabb_points[7].x, screen_space_aabb_points[7].y);
    rasterizer_->gbham(screen_space_aabb_points[7].x, screen_space_aabb_points[7].y, screen_space_aabb_points[6].x, screen_space_aabb_points[6].y);
    rasterizer_->gbham(screen_space_aabb_points[6].x, screen_space_aabb_points[6].y, screen_space_aabb_points[4].x, screen_space_aabb_points[4].y);

    // between front/back
    rasterizer_->gbham(screen_space_aabb_points[0].x, screen_space_aabb_points[0].y, screen_space_aabb_points[4].x, screen_space_aabb_points[4].y);
    rasterizer_->gbham(screen_space_aabb_points[1].x, screen_space_aabb_points[1].y, screen_space_aabb_points[5].x, screen_space_aabb_points[5].y);
    rasterizer_->gbham(screen_space_aabb_points[3].x, screen_space_aabb_points[3].y, screen_space_aabb_points[7].x, screen_space_aabb_points[7].y);
    rasterizer_->gbham(screen_space_aabb_points[2].x, screen_space_aabb_points[2].y, screen_space_aabb_points[6].x, screen_space_aabb_points[6].y);
}
