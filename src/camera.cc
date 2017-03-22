#include <math.h>

#include "global.h"
#include "Camera.h"
#include "Matrices.h"


Camera::Camera() :  fov(DEG2RAD * 90.0f),
                    aspectRatio( (float) VIEWPORT_WIDTH / VIEWPORT_HEIGHT),
                    n(1.0f),
                    f(1000.0f)
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

    float tanHalfFov = (float)tan(fov / 2.0);

    float m[16];
    m[0] = 1.0f/(tanHalfFov*aspectRatio);
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;

    m[4] = 0;
    m[5] = 1.0f/tanHalfFov;
    m[6] = 0;
    m[7] = 0;

    m[8] = 0;
    m[9] = 0;
    m[10] = -(f+n)/(f-n);
    m[11] = -1;

    m[12] = 0;
    m[13] = 0;
    m[14] = -(2*n*f)/(f-n);
    m[15] = 0;
    projectionMatrix.set(m);
}


Matrix4& Camera::getProjectionMatrix() {

    return projectionMatrix;
}


Matrix4 Camera::getViewProjection() {

    Matrix4 cameraRotation;
    Vector3 origin;

    quaternion qRot(rot);
    qRot.conjugate().getMatrix(cameraRotation, origin);

    Matrix4 cameraTranslation(
                 1,0,0,0,
                 0,1,0,0,
                 0,0,1,0,
                 -pos.x,-pos.y,-pos.z,1);

    return projectionMatrix * cameraRotation * cameraTranslation;
}
