#include "transform.h"
#include "quaternion.h"

Transform::Transform()
{
    scalevect.x = 1.0;
    scalevect.y = 1.0;
    scalevect.z = 1.0;
}

Transform::~Transform()
{
    //dtor
}

Transform::Transform(const Transform& other)
{
    //copy ctor
}


void Transform::setPosition(float x, float y, float z) {

    pos.x = x;
    pos.y = y;
    pos.z = z;
}

void Transform::setPosition(Vector3 pos) {

    this->pos.x = pos.x;
    this->pos.x = pos.y;
    this->pos.z = pos.z;
}

void Transform::movePosition(float dx, float dy, float dz) {

    pos.x += dx;
    pos.y += dy;
    pos.z += dz;
}

void Transform::movePosition(Vector3 pos) {

    this->pos.x += pos.x;
    this->pos.y += pos.y;
    this->pos.z += pos.z;
}

void Transform::scale(float scale) {

    scalevect.x = scale;
    scalevect.y = scale;
    scalevect.z = scale;
}


void Transform::scale(float sx, float sy, float sz) {

    scalevect.x = sx;
    scalevect.y = sy;
    scalevect.z = sz;
}


void Transform::scale(Vector3 scale) {

    scalevect.x = scale.x;
    scalevect.y = scale.y;
    scalevect.z = scale.z;
}


void Transform::rotate(Vector4 &axis, float angle) {

    rot.rotate(axis, angle);
}

Matrix4 Transform::getTransformation() {

    Matrix4 mpos(1,0,0,0, // 1st column
                 0,1,0,0, // 2nd column
                 0,0,1,0, // 3rd column
                 pos.x,pos.y,pos.z,1); // 4th column
    Matrix4 mrot;
    rot.getMatrix(mrot, Vector3());
    Matrix4 mscale;
    mscale.scale(scalevect.x, scalevect.y, scalevect.z);

    return mpos * mrot * mscale;
}
