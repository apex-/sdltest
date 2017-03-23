#include <math.h>

#include "global.h"
#include "camera.h"
#include "matrices.h"


Camera::Camera() :  fov_(DEG2RAD * 90.0f),
                    aspect_ratio_( (float) VIEWPORT_WIDTH / VIEWPORT_HEIGHT),
                    near_plane_(1.0f),
                    far_plane_(1000.0f)
{
      setPerspectiveProjection();
}


Camera::~Camera()
{
    //dtor
}

Camera::Camera(const Camera& other)
{
    //copy ctor
}


void Camera::setPerspectiveProjection() {

    float tanHalfFov = (float)tan(fov_ / 2.0);

    float m[16];
    m[0] = 1.0f/(tanHalfFov*aspect_ratio_);
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;

    m[4] = 0;
    m[5] = 1.0f/tanHalfFov;
    m[6] = 0;
    m[7] = 0;

    m[8] = 0;
    m[9] = 0;
    m[10] = -(far_plane_+near_plane_)/(far_plane_-near_plane_);
    m[11] = -1;

    m[12] = 0;
    m[13] = 0;
    m[14] = -(2*near_plane_*far_plane_)/(far_plane_-near_plane_);
    m[15] = 0;
    projection_matrix_.set(m);
}


Matrix4& Camera::projectionMatrix() {

    return projection_matrix_;
}


Matrix4 Camera::viewProjection() {

    Matrix4 camera_rotation;
    Vector3 origin;

    quaternion qrot(rot_);
    qrot.conjugate().getMatrix(camera_rotation, origin);

    Matrix4 camera_translation(
                 1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 -pos_.x, -pos_.y, -pos_.z, 1);

    return projection_matrix_ * camera_rotation * camera_translation;
}
