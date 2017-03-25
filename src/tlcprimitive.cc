#include "tlcprimitive.h"

#include <vector>
#include <limits>

TlcPrimitive::TlcPrimitive()
{
    aabb[0].x = std::numeric_limits<int>::max();
    aabb[7].x = std::numeric_limits<int>::min();
    aabb[0].y = std::numeric_limits<int>::max();
    aabb[7].y = std::numeric_limits<int>::min();
    aabb[0].z = std::numeric_limits<int>::min();
    aabb[7].z = std::numeric_limits<int>::max();
}

TlcPrimitive::~TlcPrimitive()
{
    //dtor
}

TlcPrimitive::TlcPrimitive(const TlcPrimitive& other)
{
    //copy ctor
}

bool TlcPrimitive::loadFromFile(const char *filename)
{
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    string err;

    bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, NULL, true);

    if (result) {
        cout << "Successfully loaded " << filename << endl;
    } else {
        cout << "Failed loading obj file" << filename << endl;
    }


    for (int i=0; i<attrib.vertices.size(); i+= 3) {
          float x = attrib.vertices[0 + i];
          float y = attrib.vertices[1 + i];
          float z = attrib.vertices[2 + i];
          Vertex v(x,y,z);
          vertexArray.push_back(v);
    }

    for (int i=0; i<shapes[0].mesh.indices.size(); i++) {
        int index = shapes[0].mesh.indices[i].vertex_index;
        indices.push_back(index);
    }

    calculateAabb();

    cout << "FACES (triangulated): " << shapes[0].mesh.num_face_vertices.size() << endl;
    cout << "INDICES (vertex+normal+uv): " << shapes[0].mesh.indices.size() << endl;
    cout << "VERTICES: (*3)" << attrib.vertices.size() << endl;
    cout << "AABB (min x,y,z) " << aabb[0] << " (max x,y,z) " << aabb[7] << endl;

    return result;
}


uint32_t TlcPrimitive::getNumberOfVertices()
{
    return vertexArray.size();
}


vector<Vertex>& TlcPrimitive::getVertexArray()
{
    return vertexArray;
}


vector<uint32_t>& TlcPrimitive::getIndices()
{
    return indices;
}


uint32_t TlcPrimitive::getNumberOfIndices()
{
    return indices.size();
}

void TlcPrimitive::calculateAabb()
{
    for (int i=0; i<vertexArray.size(); i++) {
        float x = vertexArray[i].Pos().x;
        float y = vertexArray[i].Pos().y;
        float z = vertexArray[i].Pos().z;

        if (x < aabb[0].x) {
            aabb[0].x = x;
        }
        if (x > aabb[7].x) {
            aabb[7].x = x;
        }
        if (y < aabb[0].y) {
            aabb[0].y = y;
        }
        if (y > aabb[7].y) {
            aabb[7].y = y;
        }
        if (z > aabb[0].z) {
            aabb[0].z = z;
        }
        if (z < aabb[7].z) {
            aabb[7].z = z;
        }

        aabb[0].w = 1.0f;
        aabb[7].w = 1.0f;
    }

    aabb[1].x = aabb[7].x;
    aabb[1].y = aabb[0].y;
    aabb[1].z = aabb[0].z;
    aabb[1].w = 1.0;

    aabb[2].x = aabb[0].x;
    aabb[2].y = aabb[7].y;
    aabb[2].z = aabb[0].z;
    aabb[2].w = 1.0;

    aabb[3].x = aabb[7].x;
    aabb[3].y = aabb[7].y;
    aabb[3].z = aabb[0].z;
    aabb[3].w = 1.0;

    aabb[4].x = aabb[0].x;
    aabb[4].y = aabb[0].y;
    aabb[4].z = aabb[7].z;
    aabb[4].w = 1.0;

    aabb[5].x = aabb[7].x;
    aabb[5].y = aabb[0].y;
    aabb[5].z = aabb[7].z;
    aabb[5].w = 1.0;

    aabb[6].x = aabb[0].x;
    aabb[6].y = aabb[7].y;
    aabb[6].z = aabb[7].z;
    aabb[6].w = 1.0;
}
