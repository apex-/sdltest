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
    Matrix4 vpm = camera_->ViewProjectionMatrix(); // View-projection matrix
    Matrix4 mwt = tlcinstance.Transformation().ToMatrix(); // Model to world matrix
    Matrix4 mvt = vpm * mwt; // Model to view transformation/matrix

    bbclipflags_ = 0;
    int vert_id_count = 1;

    if (!CalculateBoundingBoxFlags(primitive->getAabbModelSpace(), mvt))
        return;

    // Note that the bbclipflags_ will only be available at this point if inside_frustrum
    // is true as the method CalculateBoundingBoxParameters() will return prematurely if it
    // detects that all bounding box vertices are outside of one plane.

    vb_size_ = primitive->getNumberOfVertices();
    vbclip_size_ = 0;
    vector<Vertex> &vertices = primitive->getVertexArray();

    for (uint32_t i=0; i<vb_size_; i++) {

        // Process all vertices in one go and try to keep as much
        // data as possible in the CPU caches (deep pipelining, no conditionals etc.)

        // Transform vertex position from model-space to view space
        vb_[i].ViewSpacePos(mvt * vertices[i].Pos());
        // Transform normal from model-space to view space
        vb_[i].ViewSpaceNormal(mvt * vertices[i].Normal());
        // Calculate the screen position from the view space position calculated above
        vb_[i].CalcScreenSpacePos();
        // Set the clip flags
        vb_[i].CalcClipFlags(bbclipflags_);

        vb_[i].Id(vert_id_count++);
    }

    // assemble, (clip) and rasterize the triangles
    for (uint32_t i=0; i<primitive->getNumberOfIndices(); i+=3) {

        PipelineVertex *pv1 = &vb_[primitive->getIndices()[i]];
        PipelineVertex *pv2 = &vb_[primitive->getIndices()[i+1]];
        PipelineVertex *pv3 = &vb_[primitive->getIndices()[i+2]];

        if (!bbclipflags_ || !(pv1->ClipFlags() | pv2->ClipFlags() | pv3->ClipFlags())) {

        // back-face culling in screen space
        // TODO: Back-face culling is only performed when the triangle is completely within the frustrum
        if ((pv3->ScreenSpacePos().x - pv1->ScreenSpacePos().x) * (pv2->ScreenSpacePos().y - pv1->ScreenSpacePos().y) <
                (pv3->ScreenSpacePos().y - pv1->ScreenSpacePos().y) * (pv2->ScreenSpacePos().x - pv1->ScreenSpacePos().x))
                continue;

            // render, since no clipping necessary
            rasterizer_->rasterize(pv1, pv2, pv3);

        } else { // triangle can be culled or needs clipping

            vbclip_size_ = 0;

            if (pv1->ClipFlags() && pv2->ClipFlags() && pv3->ClipFlags()) {
                continue;
            }

            vector<PipelineVertex> clipinput;
            vector<PipelineVertex> clipoutput;

            clipinput.push_back(*pv1);
            clipinput.push_back(*pv2);
            clipinput.push_back(*pv3);

            PipelineVertex *current_vertex;
            PipelineVertex *previous_vertex;

            bool previous_inside;
            PipelineVertex clipped_vertex;

            for (int iplane=0; iplane<6; iplane++)  { // iterate over the frustrum planes (see enum FrustrumPlanes)
                previous_vertex = &clipinput[clipinput.size()-1];
                previous_inside = !(previous_vertex->ClipFlags() & (1<<iplane));

                for (int vi=0; vi<clipinput.size(); vi++) {
                    current_vertex = &clipinput[vi];
                    bool current_inside = !(current_vertex->ClipFlags() & (1<<iplane));

                    if (current_inside ^ previous_inside) {
                        ClipLerpVertex(previous_vertex, current_vertex, &clipped_vertex, iplane);
                        clipped_vertex.Id(vert_id_count++);
                        clipoutput.push_back(clipped_vertex);
                        //cout << "plane " << iplane << " id " << clipped_vertex.Id() << " added vertex: clipped vertex: " << clipped_vertex.ViewSpacePos() << endl;
                        //cout << "BASED ON from " << previous_vertex->Id() << " to " << current_vertex->Id() << endl;
                    }

                    if (current_inside) {
                        clipoutput.push_back(*current_vertex);
                        //cout << "plane " << iplane << " id " << current_vertex->Id() << " added inside vertex: " << current_vertex->ViewSpacePos() << endl;
                    }
                    previous_vertex = current_vertex;
                    previous_inside = current_inside;
                }

                std::swap(clipinput, clipoutput);
                clipoutput.clear();
            }

            for (int cv=1; cv<(clipinput.size()-1); cv++) {
                //assert(abs(clipinput[0].ViewSpacePos().x) <= clipinput[0].ViewSpacePos().w);
                //assert(abs(clipinput[0].ViewSpacePos().y) <= clipinput[0].ViewSpacePos().w);
                //assert(abs(clipinput[0].ViewSpacePos().z) <= clipinput[0].ViewSpacePos().w);

                //assert(abs(clipinput[cv].ViewSpacePos().x) <= clipinput[cv].ViewSpacePos().w);
                //assert(abs(clipinput[cv].ViewSpacePos().y) <= clipinput[cv].ViewSpacePos().w);
                //assert(abs(clipinput[cv].ViewSpacePos().z) <= clipinput[cv].ViewSpacePos().w);

                //assert(abs(clipinput[cv+1].ViewSpacePos().x) <= clipinput[cv+1].ViewSpacePos().w);
                //assert(abs(clipinput[cv+1].ViewSpacePos().y) <= clipinput[cv+1].ViewSpacePos().w);
                //assert(abs(clipinput[cv+1].ViewSpacePos().z) <= clipinput[cv+1].ViewSpacePos().w);

                //cout << "clipinput contains " << clipinput.size() << " elements " << endl;
                //cout << "rasterize " << clipinput[0].ViewSpacePos() << clipinput[cv].ViewSpacePos() << clipinput[cv+1].ViewSpacePos() << endl;
                rasterizer_->rasterize(&clipinput[0], &clipinput[cv], &clipinput[cv+1]);
            }
        }
    }

    //#ifdef _DEBUG
    DrawBoundingBox();
    //#endif

}

bool RenderPipeline::ClipLerpVertex(PipelineVertex *v1, PipelineVertex *v2, PipelineVertex *vclip, int iplane) {

    // Cohen-Sutherland algorithm (3D Version)
    float lerp = 0.0;
    float a, b;

    switch(iplane) {
        case 0: // left plane
            a = -v1->ViewSpacePos().w - v1->ViewSpacePos().x;
            b = -v2->ViewSpacePos().w - v2->ViewSpacePos().x;
            break;
        case 1:  // right plane
            a = v1->ViewSpacePos().w - v1->ViewSpacePos().x;
            b = v2->ViewSpacePos().w - v2->ViewSpacePos().x;
            break;
        case 2:  // bottom plane
            a = -v1->ViewSpacePos().w - v1->ViewSpacePos().y;
            b = -v2->ViewSpacePos().w - v2->ViewSpacePos().y;
            break;
        case 3:  // top plane
            a = v1->ViewSpacePos().w - v1->ViewSpacePos().y;
            b = v2->ViewSpacePos().w - v2->ViewSpacePos().y;
            break;
        case 4:  // near plane
            a = v1->ViewSpacePos().w - v1->ViewSpacePos().z;
            b = v2->ViewSpacePos().w - v2->ViewSpacePos().z;
            break;
        case 5:  // far plane
            a = -v1->ViewSpacePos().w - v1->ViewSpacePos().z;
            b = -v2->ViewSpacePos().w - v2->ViewSpacePos().z;
            break;
        default:
            break;
    };

    lerp = a / (a-b);

    vclip->ViewSpacePos(v1->ViewSpacePos().x + lerp * (v2->ViewSpacePos().x - v1->ViewSpacePos().x),
        v1->ViewSpacePos().y + lerp * (v2->ViewSpacePos().y - v1->ViewSpacePos().y),
        v1->ViewSpacePos().z + lerp * (v2->ViewSpacePos().z - v1->ViewSpacePos().z),
        v1->ViewSpacePos().w + lerp * (v2->ViewSpacePos().w - v1->ViewSpacePos().w));

    // OBS: Due to floating point errors the w value can be
    // a tiny bit larger than the x/y/z component which might lead to problems
    // when calculating the screen space position
    // pclip->ViewSpacePos().w *=1.000001;

    vclip->CalcClipFlags(bbclipflags_);
    vclip->CalcScreenSpacePos();

    return true;
}


bool RenderPipeline::CalculateBoundingBoxFlags(Vector4 (&aabb)[8], Matrix4 &mvpm ) {

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
