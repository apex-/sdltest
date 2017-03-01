#include <math.h>

#include "global.h"
#include "Camera.h"
#include "Matrices.h"


Camera::Camera()
{
    fov = DEG2RAD * 90.0f;
    aspectRatio = VIEWPORT_WIDTH / VIEWPORT_HEIGHT;
    zNear = 0.1f;
    zFar = 1000.0f;
}

Camera::~Camera()
{
    //dtor
}

Camera::Camera(const Camera& other)
{
    //copy ctor
}


//void Camera::getViewProjection(Matrix4& vp) const {

//    Matrix4 rot = m
//}

void Camera::getPerspectiveTransformation(Matrix4 &perspectiveTransform) const {

    float tanHalfFOV = (float)tan(fov / 2);
    float zRange = zNear - zFar;

    float m[16];

    m[0] = 1.0f / (tanHalfFOV * aspectRatio);
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0;
    m[5] = 1.0f / tanHalfFOV;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = (-zNear -zFar)/zRange;
    m[11] = 1;
    m[12] = 0;
    m[13] = 0;
    m[14] = 2 * zFar * zNear / zRange;
    m[15] = 0;
    perspectiveTransform.set(m);
}
