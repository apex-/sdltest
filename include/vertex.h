#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <stdint.h>

#include "global.h"
#include "vectors.h"
#include "matrices.h"

class Vertex
{
    public:
        Vertex();
        Vertex(const Vertex &rhs);
        Vertex(float x, float y, float z);

        inline Vector4 Pos() { return pos_; }
        inline void Pos(Vector4 pos) { pos_ = pos; }

        virtual ~Vertex();


        inline void perspectiveDivide() {
            pos_.x /= pos_.w;
            pos_.y /= pos_.w;
            pos_.z /= pos_.w;
            pos_.w /= pos_.w;
        }


        inline void toScreenCoordinates() {
                pos_.x = ((VIEWPORT_WIDTH -1) * -(pos_.x - 1)) / 2.0;
                pos_.y = ((VIEWPORT_HEIGHT -1) * -(pos_.y - 1)) / 2.0;
        }

        friend std::ostream& operator<<(std::ostream& os, const Vertex& m);

        Vector4 pos_;
        uint32_t argb_;

    protected:

    private:

};

#endif // VERTEX_H
