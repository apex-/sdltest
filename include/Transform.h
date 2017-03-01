#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "Vectors.h"
#include "Matrices.h"
#include "Quaternion.h"

class Transform
{
    public:
        Transform();
        virtual ~Transform();
        Transform(const Transform& other);

        Vector4 pos;
        quaternion rot;
        Vector4 scale;

    protected:

    private:
};

#endif // _TRANSFORM_H
