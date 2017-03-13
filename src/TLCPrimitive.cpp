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


    for (int i=0; i<attrib.vertices.size(); i+=3) {

        cout << attrib.vertices[i] << " " <<attrib.vertices[i+1] << " " << attrib.vertices[i+2] << endl;
    }


    for (int fi=0; fi<shapes[0].mesh.num_face_vertices.size(); fi++) { // iterate over tris

        for (int vi=0; vi<3; vi++) { // iterate each vertex

            tinyobj::index_t vidx = shapes[0].mesh.indices[fi+vi];
            tinyobj::index_t vidx2 = shapes[0].mesh.indices[fi+vi+1];
            tinyobj::index_t vidx3 = shapes[0].mesh.indices[fi+vi+2];

            float x = attrib.vertices[vidx.vertex_index];
            float y = attrib.vertices[vidx2.vertex_index];
            float z = attrib.vertices[vidx3.vertex_index];

            cout << "x(" << vidx.vertex_index << "): "<< x << " y(" << vidx.vertex_index+1 << "): " << y << " z(" << vidx.vertex_index+2 << "): " << z << endl;

            Vertex v(x,y,z);
            vertexArray.push_back(v);
            indices.push_back(3 * fi + vi);

            // indices.push_back(fi + vi);

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
