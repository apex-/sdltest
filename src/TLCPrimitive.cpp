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
        cout << "Successfully loaded " << filename << endl;
    } else {
        cout << "Failed loading obj file" << filename << endl;
    }

    cout << " TOTAL NUM FACE VERTICES: " << shapes[0].mesh.num_face_vertices.size() << endl;
    cout << " TOTAL NUM INDICES: " << shapes[0].mesh.indices.size() << endl;
    cout << " TOTAL NUM VERTICES: " << attrib.vertices.size() << endl;

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
