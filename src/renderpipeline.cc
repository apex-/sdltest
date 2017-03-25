#include "renderpipeline.h"
#include "camera.h"


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
    Matrix4 vpm = camera_->viewProjectionMatrix();
    Matrix4 tm = tlcinstance.GetTransform().getTransformation();
    Matrix4 mvpm = vpm * tm;

    bool is_inside_frustrum = calculateBoundingBoxParameters(primitive->getAabbModelSpace(), mvpm);

    if (is_inside_frustrum) {
            vector<Vertex> vertices = primitive->getVertexArray();

            for (uint32_t i=0; i<primitive->getNumberOfVertices(); i++) {

                // Transform Vertex from model-space to view space
                vb_[i].ViewSpacePos(mvpm * vertices[i].Pos());

                // Calculate the screen position
                vb_[i].CalcScreenSpacePos();
            }

            // TODO: Clipping

            for (uint32_t i=0; i<primitive->getNumberOfIndices(); i+=3) {
                rasterizer_->rasterize(
                                &vb_[primitive->getIndices()[i]],
                                &vb_[primitive->getIndices()[i+1]],
                                &vb_[primitive->getIndices()[i+2]]);

            }
            DrawBoundingBox();
   }

}

bool RenderPipeline::calculateBoundingBoxParameters(Vector4 (&aabb)[8], Matrix4 &mvpm ) {

    uint8_t num_vertices_outside_bb[6] = {0, 0, 0, 0, 0, 0};

    for (int i=0; i<8; i++) {
        view_space_aabb_points_[i] = mvpm * aabb[i];
        if (view_space_aabb_points_[i].x < -view_space_aabb_points_[i].w) num_vertices_outside_bb[0]++;
        if (view_space_aabb_points_[i].x > view_space_aabb_points_[i].w) num_vertices_outside_bb[1]++;
        if (view_space_aabb_points_[i].y < -view_space_aabb_points_[i].w) num_vertices_outside_bb[2]++;
        if (view_space_aabb_points_[i].y > view_space_aabb_points_[i].w) num_vertices_outside_bb[3]++;
        if (view_space_aabb_points_[i].z > view_space_aabb_points_[i].w) num_vertices_outside_bb[4]++;
        if (view_space_aabb_points_[i].z < -view_space_aabb_points_[i].w) num_vertices_outside_bb[5]++;
    }

    for (int i=0; i<6; i++) {
        if (num_vertices_outside_bb[i] > 0) {
                cout << i << " bb " <<  num_vertices_outside_bb << endl;
            return false;
        }
    }
    return true;

}

void RenderPipeline::DrawBoundingBox() {

    Vector2 screen_space_aabb_points[8];

    for (int i=0; i<8; i++) {
            screen_space_aabb_points[i].x = ((VIEWPORT_WIDTH -1) * -((view_space_aabb_points_[i].x/view_space_aabb_points_[i].w) - 1)) / 2.0f;
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
