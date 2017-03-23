#ifndef _CAMERA_H
#define _CAMERA_H

#include "matrices.h"
#include "quaternion.h"

class Camera
{
    public:
        Camera();
        virtual ~Camera();
        Camera(const Camera& other);

        void setPerspectiveProjection();

        Matrix4& projectionMatrix();
        Matrix4 viewProjection();
        Vector3 pos() const { return pos_; };
        void pos(Vector3 pos) { pos_ = pos; };
        quaternion rot() const { return rot_; };
        void rot (quaternion rot) { rot_ = rot; };

    protected:

    private:
        // view frustrum parameters
        float fov_; // [radians]
        float aspect_ratio_;
        float near_plane_; // near plane
        float far_plane_; // far plane

        Vector3 pos_; // Camera position
        quaternion rot_; // Camera rotation
        Matrix4 projection_matrix_;
};

#endif // _CAMERA_H
