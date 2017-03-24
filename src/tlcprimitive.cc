#include "tlcprimitive.h"

#include <vector>
#include <limits>

TLCPrimitive::TLCPrimitive()
{
    aabb[0].x = std::numeric_limits<int>::max();
    aabb[1].x = std::numeric_limits<int>::min();
    aabb[0].y = std::numeric_limits<int>::max();
    aabb[1].y = std::numeric_limits<int>::min();
    aabb[0].z = std::numeric_limits<int>::min();
    aabb[1].z = std::numeric_limits<int>::max();
}

TLCPrimitive::~TLCPrimitive()
{
    //dtor
}

TLCPrimitive::TLCPrimitive(const TLCPrimitive& other)
{
    //copy ctor
}

bool TLCPrimitive::loadFromFile(const char *filename)
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
    cout << "AABB (min x,y,z) " << aabb[0] << " (max x,y,z) " << aabb[1] << endl;

    return result;
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


void TLCPrimitive::calculateAabb()
{
    for (int i=0; i<vertexArray.size(); i++) {
        float x = vertexArray[i].Pos().x;
        float y = vertexArray[i].Pos().y;
        float z = vertexArray[i].Pos().z;

        if (x < aabb[0].x) {
            aabb[0].x = x;
        }
        if (x > aabb[1].x) {
            aabb[1].x = x;
        }
        if (y < aabb[0].y) {
            aabb[0].y = y;
        }
        if (y > aabb[1].y) {
            aabb[1].y = y;
        }
        if (z > aabb[0].z) {
            aabb[0].z = z;
        }
        if (z < aabb[1].z) {
            aabb[1].z = z;
        }

        aabb[0].w = 1.0f;
        aabb[1].w = 1.0f;
    }
}


bool TLCPrimitive::isBoxInsideFrustrum(Matrix4& modelViewTransform) {

    uint8_t num_vertices_outside_bb[6] = {0, 0, 0, 0, 0, 0};

    Vector4 bb_vertices[8] = {
        Vector4(aabb[0].x, aabb[0].y, aabb[0].z, 1.0),
        Vector4(aabb[1].x, aabb[0].y, aabb[0].z, 1.0),
        Vector4(aabb[0].x, aabb[1].y, aabb[0].z, 1.0),
        Vector4(aabb[1].x, aabb[1].y, aabb[0].z, 1.0),
        Vector4(aabb[0].x, aabb[0].y, aabb[1].z, 1.0),
        Vector4(aabb[1].x, aabb[0].y, aabb[1].z, 1.0),
        Vector4(aabb[0].x, aabb[1].y, aabb[1].z, 1.0),
        Vector4(aabb[1].x, aabb[1].y, aabb[1].z, 1.0)};

    for (int i=0; i<8; i++) {
        Vector4 p_mv = modelViewTransform * bb_vertices[i];

        if (p_mv.x < -p_mv.w) num_vertices_outside_bb[0]++;
        if (p_mv.x > p_mv.w) num_vertices_outside_bb[1]++;
        if (p_mv.y < -p_mv.w) num_vertices_outside_bb[2]++;
        if (p_mv.y > p_mv.w) num_vertices_outside_bb[3]++;
        if (p_mv.z > p_mv.w) num_vertices_outside_bb[4]++;
        if (p_mv.z < -p_mv.w) num_vertices_outside_bb[5]++;
    }


    for (int i=0; i<6; i++) {
        if (num_vertices_outside_bb[i] > 0) {
            return false;
        }
    }
    return true;
}
