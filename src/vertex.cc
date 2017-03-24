#include <iostream>

#include "global.h"

#include "vertex.h"
#include "matrices.h"

using namespace std;

Vertex::Vertex()
{
    //ctor
}

Vertex::Vertex(const Vertex &rhs) {

    pos_.x = rhs.pos_.x;
    pos_.y = rhs.pos_.y;
    pos_.z = rhs.pos_.z;
    pos_.w = rhs.pos_.w;
    argb_ = rhs.argb_;

}


Vertex::Vertex(float x, float y, float z) {

    this->pos_.x = x;
    this->pos_.y = y;
    this->pos_.z = z;
    this->pos_.w = 1.0;

}


std::ostream& operator<<(std::ostream& os, const Vertex& m)
{
    os << std::fixed << std::setprecision(5);
    os << "vertexpos [" << std::setw(10) << m.pos_.x << " " << std::setw(10) << m.pos_.y << " " << std::setw(10) << m.pos_.z  <<  " " << std::setw(10) << m.pos_.w << "]\n";
    os << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
    return os;
}


Vertex::~Vertex()
{
    //dtor
}
