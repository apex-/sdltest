#include <iostream>

#include "Vertex.h"


using namespace std;

Vertex::Vertex()
{
    //ctor
}


Vertex::Vertex(Vertex &rhs) {

    x = rhs.x;
    y = rhs.y;
    z = rhs.z;

}

Vertex::Vertex(float x, float y, float z) {

    this->x = x;
    this->y = y;
    this->z = z;
    this->argb = 0;
}


void Vertex::print() {

    std::cout << "x:" << x << " y:" << y << " z:" << z << " argb:" << hex << argb << std::endl;

}

Vertex::~Vertex()
{
    //dtor
}
