#include <cmath>

#include "plane.h"


Plane::~Plane()
{
    //dtor
}


float Plane::dot(Vector3 vec) const {

    return a_ * vec.x + b_ * vec.y + c_ * vec.z + d_;
}


void Plane::normalize()
{
    float norm = 1.0f/std::sqrt(a_*a_ + b_*b_ + c_*c_);
    a_*=norm;
    b_*=norm;
    c_*=norm;
    d_*=norm;
}


float Plane::dot(Vector4 vec) const {

    // OBS: We just ingnore the w component (we assume its 1)
    return a_ * vec.x + b_ * vec.y + c_ * vec.z + d_;

}
