#ifndef _CAMERA_H
#define _CAMERA_H

#include "Matrices.h"

class Camera
{
    public:
        Camera();
        virtual ~Camera();
        Camera(const Camera& other);

        void getPerspectiveTransformation(Matrix4 &perspectiveTransform) const;

    protected:

    private:
    float fov;
    float zNear;
    float zFar;
    float aspectRatio;


};

#endif // _CAMERA_H
