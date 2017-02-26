#include <iostream>

#include "Vertex.h"
#include "Matrices.h"

using namespace std;

Vertex::Vertex()
{
    //ctor
}

Vertex::Vertex(Vertex &rhs) {

    m_pos.x = rhs.m_pos.x;
    m_pos.y = rhs.m_pos.y;
    m_pos.z = rhs.m_pos.z;
    argb = rhs.argb;

}

Vertex::Vertex(float x, float y, float z) {

    this->m_pos.x = x;
    this->m_pos.y = y;
    this->m_pos.z = z;

}


void Vertex::print() {

    std::cout << "x:" << m_pos.x << " y:" << m_pos.y << " z:" << m_pos.z << " argb:" << hex << argb << std::endl;

}

Vertex::~Vertex()
{
    //dtor
}
