#ifndef _CAMERA_H
#define _CAMERA_H

#include "frustrum.h"
#include "matrices.h"
#include "quaternion.h"
#include "transform.h"

class Camera
{
    public:
        Camera();
        virtual ~Camera();
        Camera(const Camera& other);

        void SetPerspectiveProjection();
        Matrix4& ProjectionMatrix();
        Matrix4& ViewProjectionMatrix();
        Vector3 Pos() const { return transformation_.pos; }
        void Pos(Vector3 pos) { transformation_.pos = pos; is_up2date_ = false; }
        Quaternion Rot() const { return transformation_.rot; }
        void Rot(Quaternion rot) { transformation_.rot = rot; is_up2date_ = false; }
        void Move(float dx, float dy, float dz) { transformation_.movePosition(dx, dy, dz); is_up2date_ = false; }
        void Rotate(Vector4 axis, float angle) { transformation_.rotate(axis, angle); is_up2date_ = false; }


    private:
        // view frustrum parameters
        float fov_; // [radians]
        float aspect_ratio_;
        float near_plane_; // near plane
        float far_plane_; // far plane

        //Vector3 pos_; // Camera position
        //Quaternion rot_; // Camera rotation
        Transform transformation_;
        Matrix4 projection_matrix_;

        bool is_up2date_;
        Matrix4 view_projection_matrix_;
        Frustrum frustrum;

        void update(); // updates all necessary informations
};

#endif // _CAMERA_Hs
