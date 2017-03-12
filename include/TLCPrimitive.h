#ifndef TLCPRIMITIVE_H
#define TLCPRIMITIVE_H

#include <stdint.h>
#include <vector>

#include "Camera.h"
#include "Vectors.h"
#include "Vertex.h"
#include "tiny_obj_loader.h"
#include "Transform.h"

using namespace std;


enum VIEWSTATUS { inside, outside, clipped };

class TLCPrimitive
{
    public:
        TLCPrimitive();
        virtual ~TLCPrimitive();
        TLCPrimitive(const TLCPrimitive& other);

        bool loadFromFile(const char *filename);

        vector<Vertex>& getVertexArray();
        uint32_t getNumberOfVertices();
        vector<uint32_t>& getIndices();
        uint32_t getNumberOfIndices();
        Transform& getModelWorldTransform();
        VIEWSTATUS isInsideFrustrum(Camera& camera);

         Transform modelWorldTransform;
    protected:

    private:
        vector<Vertex> vertexArray;
        vector<uint32_t> indices;

        Vector4 aabb[2]; // axis-aligned bounding box

    // UV Map
    // Texture and Lighting parameters

};

#endif // TLCPRIMITIVE_H
