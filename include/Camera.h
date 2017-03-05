#ifndef _CAMERA_H
#define _CAMERA_H

#include "Matrices.h"
#include "Quaternion.h"

class Camera
{
    public:
        Camera();
        virtual ~Camera();
        Camera(const Camera& other);

        Matrix4& getPerspectiveTransformation();
        Matrix4 getViewProjection();

    protected:

    private:
    Matrix4 perspectiveTransformation;

    float fov;
    float zNear;
    float zFar;
    float aspectRatio;

    Vector3 pos;
    quaternion rot;

    Matrix4 viewProjection;
};

#endif // _CAMERA_H
