#ifndef FRUSTRUM_H
#define FRUSTRUM_H

#include <matrices.h>
#include <plane.h>

class Frustrum
{
    public:
        Frustrum();
        updatePlanes(Matrix4 &model_view_projection_matrix);
        virtual ~Frustrum();

    private:
        Plane planes[6];
};

#endif // FRUSTRUM_H
