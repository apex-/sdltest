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

class TlcPrimitive
{
    public:
        TlcPrimitive();
        virtual ~TlcPrimitive();
        TlcPrimitive(const TlcPrimitive& other);
        bool loadFromFile(const char *filename);
        vector<Vertex>& getVertexArray();
        uint32_t getNumberOfVertices();
        vector<uint32_t>& getIndices();
        uint32_t getNumberOfIndices();
        inline Vector4 (& getAabbModelSpace())[8] { return aabb; }
    protected:

    private:
        vector<Vertex> vertexArray;
        vector<uint32_t> indices;
        Vector4 aabb[8]; // axis-aligned bounding box points in model-space

        void calculateAabb();


        // UV Map
        // Texture and Lighting parameters

};

#endif // TLCPRIMITIVE_H
