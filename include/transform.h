#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "vectors.h"
#include "matrices.h"
#include "quaternion.h"

class Transform
{
    public:
        Transform();
        virtual ~Transform();
        Transform(const Transform& other);

        void setPosition(float x, float y, float z);
        void setPosition(Vector3 pos);
        void movePosition(float x, float y, float z);
        void movePosition(Vector3 pos);
        void scale(float scale);
        void scale(float sx, float sy, float sz);
        void scale(Vector3 scales);
        void rotate(Vector4 &axis, float angle);

        Matrix4 getTransformation();

        Vector3 pos;
        quaternion rot;
        Vector3 scalevect;

        //Vector3 pos;
        //quaternion rot;
        //Vector4 scale;


    protected:

    private:


};

#endif // _TRANSFORM_H