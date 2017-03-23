#ifndef TLCPRIMITIVE_H
#define TLCPRIMITIVE_H

#include <stdint.h>
#include <vector>

#include "camera.h"
#include "vectors.h"
#include "vertex.h"
#include "tinyobjloader.h"
#include "transform.h"

using namespace std;


enum CLIPPLANES { left, right, bottom, top, front, rear };

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
        inline Vector4* getAabbModelSpace() { return aabb; }
        uint8_t getAabbClipFlags(Matrix4& modelViewTransform);
        Transform modelWorldTransform;
    protected:

    private:
        vector<Vertex> vertexArray;
        vector<uint32_t> indices;
        void calculateAabb();
        Vector4 aabb[2]; // axis-aligned bounding box

    // UV Map
    // Texture and Lighting parameters

};

#endif // TLCPRIMITIVE_H