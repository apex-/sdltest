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


//vector<Vertex> TLCPrimitive::getVertexArray()
//{
//    return vertexArray;
//}


vector<uint32_t> TLCPrimitive::getIndices()
{
    return indices;
}


uint32_t TLCPrimitive::getNumberOfIndices()
{
    return indices.size();
}


bool TLCPrimitive::isInsideFrustrum() {
    // TODO: Implement
    return true;

}
