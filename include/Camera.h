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

        void setPerspectiveProjection();
        Matrix4 getViewProjection();

    protected:

    private:

        // view frustrum parameters
        float fov; // [radians]
        float aspectRatio;
        float n; // near plane
        float f; // far plane
        Matrix4 projectionMatrix;

        // camera position and orientation
        Vector3 pos;
        quaternion rot;

        Matrix4 viewProjectionMatrix;

};

#endif // _CAMERA_H
