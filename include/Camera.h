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

        Matrix4& getProjectionMatrix();
        Matrix4 getViewProjection();


        // camera position and orientation
        Vector3 pos;
        quaternion rot;

    protected:

    private:

        // view frustrum parameters
        float fov; // [radians]
        float aspectRatio;
        float n; // near plane
        float f; // far plane

        Matrix4 projectionMatrix;
};

#endif // _CAMERA_H
