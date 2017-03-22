#ifndef VERTEX_H
#define VERTEX_H

#include <iostream>
#include <stdint.h>

#include "global.h"
#include "Vectors.h"
#include "Matrices.h"

class Vertex
{
    public:
        Vertex();
        Vertex(const Vertex &rhs);
        Vertex(float x, float y, float z);
        virtual ~Vertex();


        inline void perspectiveDivide() {
            m_pos.x /= m_pos.w;
            m_pos.y /= m_pos.w;
            m_pos.z /= m_pos.w;
            m_pos.w /= m_pos.w;
        }


        inline void toScreenCoordinates() {
                m_pos.x = ((VIEWPORT_WIDTH -1) * -(m_pos.x - 1)) / 2.0;
                m_pos.y = ((VIEWPORT_HEIGHT -1) * -(m_pos.y - 1)) / 2.0;
        }

        friend std::ostream& operator<<(std::ostream& os, const Vertex& m);

        Vector4 m_pos;
        uint32_t argb;

    protected:

    private:

};

#endif // VERTEX_H
