#ifndef _CAMERA_H
#define _CAMERA_H

#include "Matrices.h"

class Camera
{
    public:
        Camera();
        virtual ~Camera();
        Camera(const Camera& other);

        Matrix4& getPerspectiveTransformation();

    protected:

    private:
    Matrix4 perspectiveTransformation;
    float fov;
    float zNear;
    float zFar;
    float aspectRatio;


};

#endif // _CAMERA_H
