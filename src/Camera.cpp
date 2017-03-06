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

    float tanHalfFOV = (float)tan(fov / 2.0);
    //float zRange = zNear - zFar;
    float m[16];
//    m[0] = 1.0f / (tanHalfFOV * aspectRatio);
//    m[1] = 0;
//    m[2] = 0;
//    m[3] = 0;
//    m[4] = 0;
//    m[5] = 1.0f / tanHalfFOV;
//    m[6] = 0;
//    m[7] = 0;
//    m[8] = 0;
//    m[9] = 0;
//    m[10] = (-zNear -zFar)/zRange;
//    m[11] = 1;
//    m[12] = 0;
//    m[13] = 0;
//    m[14] = 2 * zFar * zNear / zRange;
//    m[15] = 0;


    float zRange = zFar - zNear;
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
    m[10] = -zFar/zRange;
    m[11] = -1;
    m[12] = 0;
    m[13] = 0;
    m[14] = zFar * zNear / zRange;
    m[15] = 0;


    perspectiveTransformation.set(m);

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

Matrix4& Camera::getPerspectiveTransformation() {

    return perspectiveTransformation;
}


Matrix4 Camera::getViewProjection() {

    Matrix4 cameraRotation;
    Vector3 origin;

    rot.conjugate().getMatrix(cameraRotation, origin);

    // negative translation
    Matrix4 cameraTranslation(1,0,0,0, // 1st column
                 0,1,0,0, // 2nd column
                 0,0,1,0, // 3rd column
                 -pos.x,-pos.y,-pos.z,1); // 4th column


    viewProjection = perspectiveTransformation * cameraRotation * cameraTranslation;

    return viewProjection;
    //return perspectiveTransformation * cameraRotation * cameraTranslation;

}
