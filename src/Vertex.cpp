#include <iostream>

#include "global.h"

#include "Vertex.h"
#include "Matrices.h"

using namespace std;

Vertex::Vertex()
{
    //ctor
}

Vertex::Vertex(const Vertex &rhs) {

    m_pos.x = rhs.m_pos.x;
    m_pos.y = rhs.m_pos.y;
    m_pos.z = rhs.m_pos.z;
    m_pos.w = rhs.m_pos.w;
    argb = rhs.argb;

}


Vertex::Vertex(float x, float y, float z) {

    this->m_pos.x = x;
    this->m_pos.y = y;
    this->m_pos.z = z;
    this->m_pos.w = 1.0;

}


std::ostream& operator<<(std::ostream& os, const Vertex& m)
{
    os << std::fixed << std::setprecision(5);
    os << "vertexpos [" << std::setw(10) << m.m_pos.x << " " << std::setw(10) << m.m_pos.y << " " << std::setw(10) << m.m_pos.z  <<  " " << std::setw(10) << m.m_pos.w << "]\n";
    os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
    return os;
}


Vertex::~Vertex()
{
    //dtor
}
