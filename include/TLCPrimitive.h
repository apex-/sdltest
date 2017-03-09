#ifndef TLCPRIMITIVE_H
#define TLCPRIMITIVE_H

#include <stdint.h>
#include <vector>

#include "Vectors.h"
#include "Vertex.h"
#include "Transform.h"

using namespace std;

class TLCPrimitive
{
    public:
        TLCPrimitive();
        virtual ~TLCPrimitive();
        TLCPrimitive(const TLCPrimitive& other);

//        vector<Vertex> getVertexArray();
        uint32_t getNumberOfVertices();
        vector<uint32_t> getIndices();
        uint32_t getNumberOfIndices();
        bool isInsideFrustrum();

    protected:

    private:
        vector<Vertex> vertexArray;
        vector<uint32_t> indices;
        Transform modelWorldTransform;
        Vector3 aabb[2]; // axis-aligned bounding box

    // UV Map
    // Texture and Lighting parameters

};

#endif // TLCPRIMITIVE_H
