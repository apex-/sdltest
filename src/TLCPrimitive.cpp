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

bool TLCPrimitive::loadFromFile(const char *filename)
{
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    string err;

    bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, NULL, true);

    if (result) {
        cout << "success" << endl;
    } else {
        cout << "failed loading obj file" << endl;
    }


cout << " TOTAL NUM FACE VERTICES: " << shapes[0].mesh.num_face_vertices.size() << endl;
cout << " TOTAL NUM INDICES: " << shapes[0].mesh.indices.size() << endl;
cout << " TOTAL NUM VERTICES: " << attrib.vertices.size() << endl;

    for (int fi=0; fi<shapes[0].mesh.num_face_vertices.size(); fi++) { // iterate over tris
        for (int vi=0; vi<3; vi++) { // iterate each vertex
            tinyobj::index_t vidx = shapes[0].mesh.indices[fi+vi];
            float x = attrib.vertices[vidx.vertex_index];
            float y = attrib.vertices[vidx.vertex_index+1];
            float z = attrib.vertices[vidx.vertex_index+2];
            Vertex v(x,y,z);
            vertexArray.push_back(v);
            indices.push_back(3 * fi + vi);
        }
    }
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


VIEWSTATUS TLCPrimitive::isInsideFrustrum(Camera& camera) {

    Matrix4 vp = camera.getViewProjection();
    Matrix4 mvp = vp * modelWorldTransform.getTransformation();

    Vector4 mvpbb0 = mvp * aabb[0];
    Vector4 mvpbb1 = mvp * aabb[1];

    return inside;

}
