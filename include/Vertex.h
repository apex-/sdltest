#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <stdint.h>
#include "Vectors.h"
#include "Matrices.h"

class Vertex
{
    public:
        Vertex();
        Vertex(const Vertex &rhs);
        Vertex(float x, float y, float z);
        virtual ~Vertex();

        void perspectiveDivide();
        void toScreenCoordinates();

        friend std::ostream& operator<<(std::ostream& os, const Vertex& m);

        Vector4 m_pos;
        uint32_t argb;

    protected:

    private:

};

#endif // VERTEX_H
