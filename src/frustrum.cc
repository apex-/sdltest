#include "frustrum.h"

Frustrum::Frustrum()
{

}

Frustrum::~Frustrum()
{
    //dtor
}


void Frustrum::updatePlanes(Matrix4 &model_view_projection_matrix)
{

    // extract Frustrum planes from view projection matrix
    const float* m = model_view_projection_matrix.get();
    planes[0].set(m[0]+m[12], m[1]+m[13], m[2]+m[14], m[3]+m[15]); //left
    planes[1].set(-m[0]+m[12], -m[1]+m[13], -m[2]+m[14], -m[3]+m[15]); //right
    planes[2].set(m[4]+m[12], m[5]+m[13], m[6]+m[14], m[7]+m[15]); // bottom
    planes[3].set(-m[4]+m[12], -m[5]+m[13], -m[6]+m[14], -m[7]+m[15]); // top
    planes[4].set(m[8]+m[12], m[9]+m[13], m[10]+m[14], m[11]+m[15]); // near
    planes[5].set(-m[8]+m[12], -m[9]+m[13], -m[10]+m[14], -m[11]+m[15]); // far

    planes[0].normalize();
    planes[1].normalize();
    planes[2].normalize();
    planes[3].normalize();
    planes[4].normalize();
    planes[5].normalize();
}



int boxInFrustrum(Vector4 aabb)
{



}
