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

        Vector4 m_pos;
        quaternion m_rot;
        Vector4 m_scale;

    protected:

    private:
};

#endif // _TRANSFORM_H
