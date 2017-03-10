#include "TLCPrimitive.h"

#include <vector>

TLCPrimitive::TLCPrimitive()
{
    //ctor
}

TLCPrimitive::~TLCPrimitive()
{
    //dtor
}

TLCPrimitive::TLCPrimitive(const TLCPrimitive& other)
{
    //copy ctor
}


uint32_t TLCPrimitive::getNumberOfVertices()
{
    return vertexArray.size();
}


vector<Vertex>& TLCPrimitive::getVertexArray()
{
    return vertexArray;
}


vector<uint32_t>& TLCPrimitive::getIndices()
{
    return indices;
}


uint32_t TLCPrimitive::getNumberOfIndices()
{
    return indices.size();
}


Transform& TLCPrimitive::getModelWorldTransform()
{
    return modelWorldTransform;
}


VIEWSTATUS TLCPrimitive::isInsideFrustrum(Camera& camera) {

    Matrix4 vp = camera.getViewProjection();
    Matrix4 mvp = vp * modelWorldTransform.getTransformation();

    Vector4 mvpbb0 = mvp * aabb[0];
    Vector4 mvpbb1 = mvp * aabb[0];

    return inside;

}
